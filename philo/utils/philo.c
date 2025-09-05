/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

#ifdef FORK_CHECK
static void	report_fork_violation(t_sim *sim, int fork_idx, int current, int attempted)
{
	fprintf(stderr,
		"[FORK_CHECK] VIOLATION: fork %d already owned by %d (attempt by %d)\n",
		fork_idx, current, attempted);
	sim->stop = 1;
}

static void	mark_fork_acquire(t_philo *p, int fork_idx)
{
	int owner;

	owner = p->sim->fork_owner[fork_idx];
	if (owner != -1 && owner != p->id)
		report_fork_violation(p->sim, fork_idx, owner, p->id);
	else
		p->sim->fork_owner[fork_idx] = p->id;
}

static void	mark_fork_release(t_philo *p, int fork_idx)
{
	int owner;

	owner = p->sim->fork_owner[fork_idx];
	if (owner == p->id || owner == -1)
		p->sim->fork_owner[fork_idx] = -1;
	else
		report_fork_violation(p->sim, fork_idx, owner, p->id);
}
#endif

static void	philo_init(int *values, t_philo *philos, t_sim *sim);

static void	lock_forks(t_philo *p)
{
	int left = p->left_fork;
	int right = p->right_fork;
	int first;
	int second;

	// Parity-based order to reduce starvation (deadlock still prevented by waiter)
	if (p->id % 2 == 0)
	{
		first = right;
		second = left;
	}
	else
	{
		first = left;
		second = right;
	}
	DBG(p->sim, p->id, "forks", "lock order %d -> %d", first, second);
	// ...existing code...
	if (first == second)
	{
		pthread_mutex_lock(&p->sim->forks[first]);
#ifdef FORK_CHECK
		mark_fork_acquire(p, first);
#endif
		log_status(p->sim, p->id, "has taken a fork");
		return ;
	}
	pthread_mutex_lock(&p->sim->forks[first]);
#ifdef FORK_CHECK
	mark_fork_acquire(p, first);
#endif
	log_status(p->sim, p->id, "has taken a fork");
	pthread_mutex_lock(&p->sim->forks[second]);
#ifdef FORK_CHECK
	mark_fork_acquire(p, second);
#endif
	log_status(p->sim, p->id, "has taken a fork");
}

static void	unlock_forks(t_philo *p)
{
	int left = p->left_fork;
	int right = p->right_fork;

	if (left == right)
	{
#ifdef FORK_CHECK
		mark_fork_release(p, left);
#endif
		pthread_mutex_unlock(&p->sim->forks[left]);
		return ;
	}
	DBG(p->sim, p->id, "forks", "unlock order %s first",
		(p->id % 2 == 0) ? "left" : "right");
	if (p->id % 2 == 0)
	{
#ifdef FORK_CHECK
		mark_fork_release(p, left);
		mark_fork_release(p, right);
#endif
		pthread_mutex_unlock(&p->sim->forks[left]);
		pthread_mutex_unlock(&p->sim->forks[right]);
	}
	else
	{
#ifdef FORK_CHECK
		mark_fork_release(p, right);
		mark_fork_release(p, left);
#endif
		pthread_mutex_unlock(&p->sim->forks[right]);
		pthread_mutex_unlock(&p->sim->forks[left]);
	}
}

static int	best_pending_by_priority(t_sim *sim)
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

static void	waiter_take_slot(t_philo *ph)
{
	t_sim *sim = ph->sim;
	int    id = ph->id;

	while (!sim_get_stop(sim))
	{
		pthread_mutex_lock(&sim->slot);
		if (sim_get_stop(sim))
		{
			pthread_mutex_unlock(&sim->slot);
			break ;
		}
		// Only block by urgent if that urgent philosopher is actually pending
		if (sim->urgent_id != -1 && sim->urgent_id != id
			&& sim->pending[sim->urgent_id - 1])
			DBG(sim, id, "waiter", "blocked by urgent=%d (slots=%d,next=%d)",
				sim->urgent_id, sim->slots, sim->next_id);
		if (sim->slots > 0 && (sim->urgent_id == -1 || sim->urgent_id == id
				|| !sim->pending[sim->urgent_id - 1]))
		{
			int best = best_pending_by_priority(sim);
			int allow = 0;
			if (sim->urgent_id == id || (best != -1 && best == id)
				|| sim->next_id == id || sim->pending[sim->next_id - 1] == 0)
				allow = 1;
			if (allow)
			{
				sim->slots--;
				if (sim->urgent_id == id)
					sim->urgent_id = -1;
				int k, found = 0;
				for (k = 0; k < sim->num_philos; k++)
				{
					int cand = ((id + k) % sim->num_philos) + 1;
					if (sim->pending[cand - 1])
					{
						sim->next_id = cand;
						found = 1;
						break ;
					}
				}
				if (!found)
					sim->next_id = (id % sim->num_philos) + 1;
				sim->pending[id - 1] = 0;
				DBG(sim, id, "waiter", "GRANTED slot (slots now %d, next=%d)",
					sim->slots, sim->next_id);
				pthread_mutex_unlock(&sim->slot);
				break ;
			}
			else
				DBG(sim, id, "waiter", "DENY (slots=%d,next=%d,pending[next]=%d)",
				 sim->slots, sim->next_id, sim->pending[sim->next_id - 1]);
		}
		pthread_mutex_unlock(&sim->slot);
		usleep(1000);
	}
}

static void	waiter_give_slot(t_sim *sim)
{
	pthread_mutex_lock(&sim->slot);
	sim->slots++;
	pthread_mutex_unlock(&sim->slot);
}

static void	*philo_routine(void *arg)
{
	t_philo *ph = (t_philo *)arg;

	// Stagger start differently for odd/even counts
	if (ph->sim->num_philos % 2 == 0)
	{
		if (ph->id % 2 == 0)
			usleep((ph->time_to_eat / 2) * 1000);
	}
	else
	{
		// Stronger desync for odd counts: delay even IDs by full eat time
		if (ph->id % 2 == 0)
			usleep(ph->time_to_eat * 1000);
	}

	// Single philosopher edge case: can never eat
	if (ph->sim->num_philos == 1)
	{
		DBG(ph->sim, ph->id, "start", "single philosopher path");
		waiter_take_slot(ph);
		lock_forks(ph);
		ft_usleep_ms(ph->time_to_die + 1, ph->sim);
		unlock_forks(ph);
		waiter_give_slot(ph->sim);
		return (NULL);
	}
	while (!sim_get_stop(ph->sim))
	{
		log_status(ph->sim, ph->id, "is thinking");
		pthread_mutex_lock(&ph->sim->slot);
		ph->sim->pending[ph->id - 1] = 1;
		ph->sim->pending_since[ph->id - 1] = get_time_ms();
		DBG(ph->sim, ph->id, "waiter", "wants slot (slots=%d,urgent=%d,next=%d)",
			ph->sim->slots, ph->sim->urgent_id, ph->sim->next_id);
		pthread_mutex_unlock(&ph->sim->slot);

		waiter_take_slot(ph);
		if (sim_get_stop(ph->sim))
			break ;

		lock_forks(ph);
		if (sim_get_stop(ph->sim))
		{
			unlock_forks(ph);
			waiter_give_slot(ph->sim);
			break ;
		}
		pthread_mutex_lock(&ph->sim->state);
		ph->last_meal_time = get_time_ms();
		ph->last_meal_us = get_time_us();
		pthread_mutex_unlock(&ph->sim->state);
		log_status(ph->sim, ph->id, "is eating");
		ft_usleep_ms(ph->time_to_eat, ph->sim);

		pthread_mutex_lock(&ph->sim->state);
		ph->meals_eaten++;
		DBG(ph->sim, ph->id, "meal", "meals_eaten=%d", ph->meals_eaten);
		if (ph->sim->meals_required > 0
			&& ph->meals_eaten >= ph->sim->meals_required)
		{
			ph->sim->finished++;
			DBG(ph->sim, ph->id, "meal", "finished=%d/%d",
				ph->sim->finished, ph->sim->num_philos);
			if (ph->sim->finished >= ph->sim->num_philos)
				sim_set_stop(ph->sim);
		}
		pthread_mutex_unlock(&ph->sim->state);

		unlock_forks(ph);
		waiter_give_slot(ph->sim);

		log_status(ph->sim, ph->id, "is sleeping");
		ft_usleep_ms(ph->time_to_sleep, ph->sim);
		if (ph->sim->num_philos % 2 == 1)
		{
			long extra = ph->time_to_eat;
			if (extra < 1) extra = 1;
			DBG(ph->sim, ph->id, "desync", "extra think %ldms", extra);
			ft_usleep_ms(extra, ph->sim);
		}
	}
	return (NULL);
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

static void	philo_init(int *values, t_philo *philos, t_sim *sim)
{
	int	num_philos = values[0];
	int	i = 0;

	while (i < num_philos)
	{
		philos[i].id = i + 1;
		philos[i].time_to_die = values[1];
		philos[i].time_to_eat = values[2];
		philos[i].time_to_sleep = values[3];
		philos[i].state = THINKING;
		philos[i].meals_required = values[4];
		philos[i].left_fork = i;
		philos[i].right_fork = (i + 1) % num_philos;
		philos[i].last_meal_time = 0;
		philos[i].last_meal_us = 0;
		philos[i].meals_eaten = 0;
		philos[i].thread = NULL;
		philos[i].sim = sim;
		{
			struct sched_param p;
			int minp = sched_get_priority_min(SCHED_RR);
			int maxp = sched_get_priority_max(SCHED_RR);
			int mid = minp + (maxp - minp) / 2;
			ft_pthread_attr_init(&philos[i].attr);
			ft_pthread_attr_setschedpolicy(&philos[i].attr, SCHED_RR);
			ft_pthread_attr_setinheritsched(&philos[i].attr, PTHREAD_EXPLICIT_SCHED);
			p.sched_priority = mid;
			ft_pthread_attr_setschedparam(&philos[i].attr, &p);
		}
		i++;
	}
}
