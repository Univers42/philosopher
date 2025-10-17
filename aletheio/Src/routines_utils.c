/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routines_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/15 17:02:54 by alepinto          #+#    #+#             */
/*   Updated: 2025/10/16 17:13:25 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Inc/philo.h"

void release_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->l_fork);
	pthread_mutex_unlock(philo->r_fork);
	philo->has_both_forks = 0;
	// give back waiter seat
	pthread_mutex_lock(&philo->sim->room_lock);
	philo->sim->room++;
	pthread_mutex_unlock(&philo->sim->room_lock);
}

static inline pthread_mutex_t *first_fork_for(t_philo *p)
{
	return (p->id % 2 == 0) ? p->r_fork : p->l_fork;
}

static inline pthread_mutex_t *second_fork_for(t_philo *p)
{
	return (p->id % 2 == 0) ? p->l_fork : p->r_fork;
}

static long time_remaining_to_die(t_philo *philo)
{
	long now;
	long last_meal;

	now = get_current_time_ms();
	pthread_mutex_lock(&philo->sim->meal_lock);
	last_meal = philo->last_meal;
	pthread_mutex_unlock(&philo->sim->meal_lock);
	return (philo->sim->time_to_die - (now - last_meal));
}

void taken_forks(t_philo *philo)
{
	pthread_mutex_t *first;
	pthread_mutex_t *second;
	int has_first;

	if (philo_has_died(philo))
		return;

	// waiter: acquire a seat (N - 1)
	while (!philo_has_died(philo))
	{
		pthread_mutex_lock(&philo->sim->room_lock);
		if (philo->sim->room > 0)
		{
			philo->sim->room--;
			pthread_mutex_unlock(&philo->sim->room_lock);
			break;
		}
		pthread_mutex_unlock(&philo->sim->room_lock);
		usleep(100);
	}
	if (philo_has_died(philo))
		return;

	first = first_fork_for(philo);
	second = second_fork_for(philo);
	has_first = 0;

	while (!philo_has_died(philo))
	{
		pthread_mutex_lock(first);
		has_first = 1;
		if (philo_has_died(philo))
			break;
		print_status(philo, "has taken a fork", CYAN);

		// If we're close to starving, block on second fork (safe: waiter avoids deadlock)
		if (time_remaining_to_die(philo) <= philo->sim->time_to_eat + 2)
		{
			pthread_mutex_lock(second);
			print_status(philo, "has taken a fork", CYAN);
			philo->has_both_forks = 1;
			return;
		}
		// Otherwise attempt non-blocking second fork to reduce contention
		if (pthread_mutex_trylock(second) == 0)
		{
			print_status(philo, "has taken a fork", CYAN);
			philo->has_both_forks = 1;
			return;
		}
		// Could not get second fork: release first and back off
		pthread_mutex_unlock(first);
		has_first = 0;
		usleep(200);
	}

	// Early exit (death detected) before acquiring both forks:
	if (has_first)
		pthread_mutex_unlock(first);
	pthread_mutex_lock(&philo->sim->room_lock);
	philo->sim->room++;
	pthread_mutex_unlock(&philo->sim->room_lock);
}

void check_and_print_full(t_philo *philo)
{
	long timestamp;
	int required_meals;

	required_meals = philo->sim->number_of_times_each_philosopher_must_eat;
	if (required_meals != -1 && philo->meals_eaten == required_meals)
	{
		pthread_mutex_lock(&philo->sim->full_lock);
		philo->sim->full_philos++;
		if (philo->sim->full_philos == philo->sim->num_of_philos)
		{
			timestamp = get_current_time_ms() - philo->sim->start_time;
			pthread_mutex_lock(&philo->sim->write_lock);
			printf(YELLOW "[%ld] All philosophers have eaten %d times "
						  "🐷\n" RESET,
				   timestamp,
				   required_meals);
			pthread_mutex_unlock(&philo->sim->write_lock);
		}
		pthread_mutex_unlock(&philo->sim->full_lock);
	}
}

void create_threads(t_sim *sim, pthread_t *monitor_thread)
{
	int i;

	sim->start_time = get_current_time_ms();
	i = 0;
	while (i < sim->num_of_philos)
	{
		sim->philos[i].last_meal = sim->start_time;
		i++;
	}
	if (pthread_create(monitor_thread, NULL, monitor_func, sim) != 0)
		clean_up_all("Failed to create monitor thread\n", sim);
	i = 0;
	while (i < sim->num_of_philos)
	{
		if (pthread_create(&sim->philos[i].thread, NULL, philo_routine,
						   &sim->philos[i]) != 0)
			clean_up_all("Failed to create philosopher thread\n", sim);
		i++;
	}
}

void join_threads(t_sim *sim, pthread_t *monitor_thread)
{
	int i;

	if (pthread_join(*monitor_thread, NULL) != 0)
		clean_up_all("Thread join error\n", sim);
	i = 0;
	while (i < sim->num_of_philos)
	{
		if (pthread_join(sim->philos[i].thread, NULL) != 0)
			clean_up_all("Thread join error\n", sim);
		i++;
	}
}
