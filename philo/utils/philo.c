/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

int	best_pending_by_priority(t_sim *sim)
{
	int					i;
	int					best_id;
	int					best_prio;
	struct sched_param	param;
	t_philo				*philos;

	best_id = -1;
	best_prio = -2147483648;
	philos = sim->philos;
	i = 0;
	while (i < sim->num_philos)
	{
		if (sim->pending[i])
		{
			if (ft_pthread_attr_getschedparam(&philos[i].attr, &param) == 0)
			{
				if (param.sched_priority > best_prio)
				{
					best_prio = param.sched_priority;
					best_id = philos[i].id;
				}
			}
		}
		i++;
	}
	return (best_id);
}

int main(int argc, char **argv)
{
	int				values[5] = {0, 0, 0, 0, -1};
	t_philo			philo[N];
	pthread_t		thread_id[N];
	pthread_attr_t	attr;
	int				i;
	int				num_philos;
	int				detachstate;
	t_sim			sim;

	argcheck(argc, argv, values);
	num_philos = values[0];
	if (num_philos > N)
	{
		printf("Error: Too many philosophers (max %d)\n", N);
		return (1);
	}

	// init sim
	sim.num_philos = num_philos;
	sim.meals_required = values[4];
	sim.stop = 0;
	sim.finished = 0;
	sim.forks = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * num_philos);
	if (!sim.forks)
		return (printf("Error: malloc failed\n"), 1);
	i = 0;
	while (i < num_philos)
		pthread_mutex_init(&sim.forks[i++], NULL);
	pthread_mutex_init(&sim.print, NULL);
	pthread_mutex_init(&sim.state, NULL);
	sim.slots = (num_philos > 1) ? (num_philos - 1) : 1;
	pthread_mutex_init(&sim.slot, NULL);
	sim.urgent_id = -1;
	sim.next_id = 1;
	sim.pending = (int *)malloc(sizeof(int) * num_philos);
	if (!sim.pending)
		return (printf("Error: malloc failed\n"), 1);
	sim.pending_since = (long *)malloc(sizeof(long) * num_philos);
	if (!sim.pending_since)
		return (printf("Error: malloc failed\n"), 1);
	i = 0;
	while (i < num_philos)
	{
		sim.pending[i] = 0;
		sim.pending_since[i] = 0;
		i++;
	}
#ifdef FORK_CHECK
	sim.fork_owner = (int *)malloc(sizeof(int) * num_philos);
	if (!sim.fork_owner)
		return (printf("Error: malloc failed\n"), 1);
	i = 0;
	while (i < num_philos)
		sim.fork_owner[i++] = -1;
#endif

	philo_init(values, philo, &sim);
	sim.philos = philo;

	if (ft_pthread_attr_init(&attr) != 0)
		return (printf("Error: ft_pthread_attr_init failed\n"), 1);
	ft_pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	if (ft_pthread_attr_validate(&attr) != 0)
		printf("Warning: attribute validation failed\n");
	ft_pthread_attr_print(&attr);
	ft_pthread_attr_getdetachstate(&attr, &detachstate);

	sim.start_ms = get_time_ms();
	sim.start_us = get_time_us();
	i = 0;
	while (i < num_philos)
	{
		philo[i].last_meal_time = sim.start_ms;
		philo[i].last_meal_us = sim.start_us;
		if (pthread_create(&thread_id[i], NULL, philo_routine, &philo[i]) != 0)
		{
			printf("Error: pthread_create failed for philosopher %d\n", i + 1);
			ft_pthread_attr_destroy(&attr);
			return (1);
		}
		if (detachstate == PTHREAD_CREATE_DETACHED)
			pthread_detach(thread_id[i]);
		i++;
	}

	// High-resolution monitor loop
	while (!sim_get_stop(&sim))
	{
		long	now_us = get_time_us();
		long	min_deadline_us = -1;
		int		i_scan = 0;

		while (i_scan < num_philos && !sim_get_stop(&sim))
		{
			long	last_us;
			int		ttd_ms;
			long	deadline_us;
			long	elapsed_ms;

			pthread_mutex_lock(&sim.state);
			last_us = philo[i_scan].last_meal_us;
			ttd_ms = philo[i_scan].time_to_die;
			pthread_mutex_unlock(&sim.state);

			deadline_us = last_us + (long)ttd_ms * 1000L;
			elapsed_ms = (now_us - last_us) / 1000L;
			if (elapsed_ms >= ttd_ms)
			{
				pthread_mutex_lock(&sim.print);
				if (!sim_get_stop(&sim))
				{
					long actual_ms = (now_us - sim.start_us) / 1000L;
					long expected_ms = (deadline_us - sim.start_us) / 1000L;
					long delta_ms = actual_ms - expected_ms;
					death_timing_log(philo[i_scan].id, expected_ms, actual_ms, delta_ms);
					printf("%ld %d died\n", actual_ms, philo[i_scan].id);
					sim_set_stop(&sim);
				}
				pthread_mutex_unlock(&sim.print);
				break ;
			}
			if (min_deadline_us == -1 || deadline_us < min_deadline_us)
				min_deadline_us = deadline_us;
			i_scan++;
		}
		if (sim_get_stop(&sim))
			break ;
		if (min_deadline_us == -1)
		{
			usleep(500);
			continue ;
		}
		now_us = get_time_us();
		long remaining_us = min_deadline_us - now_us;
		if (remaining_us <= 0)
			continue;
		long sleep_us = remaining_us / 2;
		if (sleep_us > 800) sleep_us = 800;
		if (sleep_us < 100) sleep_us = 100;
		usleep(sleep_us);
	}

	if (detachstate == PTHREAD_CREATE_JOINABLE)
	{
		i = 0;
		while (i < num_philos)
			pthread_join(thread_id[i++], NULL);
	}

	ft_pthread_attr_destroy(&attr);
	i = 0;
	while (i < num_philos)
		pthread_mutex_destroy(&sim.forks[i++]);
	pthread_mutex_destroy(&sim.print);
	pthread_mutex_destroy(&sim.state);
	pthread_mutex_destroy(&sim.slot);
	free(sim.pending_since);
	free(sim.pending);
	free(sim.forks);
#ifdef FORK_CHECK
	free(sim.fork_owner);
#endif
	return (0);
}

