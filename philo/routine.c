/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                                            */
/* ************************************************************************** */

#include "utils/philosopher.h"
#include <stdbool.h>

static bool	has_one_philo(t_philo *ph)
{
	if (ph->sim->num_philos == 1)
	{
		DBG(ph->sim, ph->id, "start", "single philosopher path");
		waiter_take_slot(ph);
		lock_forks(ph);
		ft_usleep_ms(ph->time_to_die + 1, ph->sim);
		unlock_forks(ph);
		waiter_give_slot(ph->sim);
		return (true);
	}
	return (false);
}

static void	stagger_start(t_philo *ph)
{
	if (ph->sim->num_philos % 2 == 0)
	{
		if (ph->id % 2 == 0)
			usleep((ph->time_to_eat / 2) * 1000);
	}
	else
	{
		if (ph->id % 2 == 0)
			usleep(ph->time_to_eat * 1000);
	}
}

static void	mark_pending_and_request_slot(t_philo *ph)
{
	pthread_mutex_lock(&ph->sim->slot);
	ph->sim->pending[ph->id - 1] = 1;
	ph->sim->pending_since[ph->id - 1] = get_time_ms();
	DBG(ph->sim, ph->id, "waiter", "wants slot (slots=%d,urgent=%d,next=%d)",
		ph->sim->slots, ph->sim->urgent_id, ph->sim->next_id);
	pthread_mutex_unlock(&ph->sim->slot);
	waiter_take_slot(ph);
}

static bool	has_reached_quota(t_philo *ph)
{
	bool	done;

	if (ph->sim->meals_required <= 0)
		return (false);
	pthread_mutex_lock(&ph->sim->state);
	done = (ph->meals_eaten >= ph->sim->meals_required);
	pthread_mutex_unlock(&ph->sim->state);
	return (done);
}

static void	post_eat_updates(t_philo *ph)
{
	pthread_mutex_lock(&ph->sim->state);
	ph->meals_eaten++;
	DBG(ph->sim, ph->id, "meal", "meals_eaten=%d", ph->meals_eaten);
	if (ph->sim->meals_required > 0
		&& ph->meals_eaten == ph->sim->meals_required)
	{
		ph->sim->finished++;
		DBG(ph->sim, ph->id, "meal", "finished=%d/%d",
			ph->sim->finished, ph->sim->num_philos);
		if (ph->sim->finished >= ph->sim->num_philos)
			sim_set_stop(ph->sim);
	}
	pthread_mutex_unlock(&ph->sim->state);
}

static void	odd_desync_extra_think(t_philo *ph)
{
	long	extra;

	if (ph->sim->num_philos % 2 == 1)
	{
		extra = ph->time_to_eat;
		if (extra < 1)
			extra = 1;
		DBG(ph->sim, ph->id, "desync", "extra think %ldms", extra);
		ft_usleep_ms(extra, ph->sim);
	}
}

static bool	eat_once(t_philo *ph)
{
	if (sim_get_stop(ph->sim))
		return (false);
	lock_forks(ph);
	if (sim_get_stop(ph->sim))
	{
		unlock_forks(ph);
		waiter_give_slot(ph->sim);
		return (false);
	}
	pthread_mutex_lock(&ph->sim->state);
	ph->last_meal_time = get_time_ms();
	ph->last_meal_us = get_time_us();
	pthread_mutex_unlock(&ph->sim->state);
	log_status(ph->sim, ph->id, "is eating");
	ft_usleep_ms(ph->time_to_eat, ph->sim);
	post_eat_updates(ph);
	unlock_forks(ph);
	waiter_give_slot(ph->sim);
	return (true);
}

static bool	philo_cycle(t_philo *ph)
{
	if (has_reached_quota(ph))
		return (false);
	log_status(ph->sim, ph->id, "is thinking");
	mark_pending_and_request_slot(ph);
	if (!eat_once(ph))
		return (false);
	log_status(ph->sim, ph->id, "is sleeping");
	ft_usleep_ms(ph->time_to_sleep, ph->sim);
	odd_desync_extra_think(ph);
	return (true);
}

/**
 * Stagger start differently for odd/even counts
 * Stronger desync for odd counts: delay even IDs by full eat time
 * Single philosopher edge case: can never eat
 */
void	*philo_routine(void *arg)
{
	t_philo	*ph;

	ph = (t_philo *)arg;
	stagger_start(ph);
	if (has_one_philo(ph))
		return (NULL);
	while (!sim_get_stop(ph->sim))
	{
		if (!philo_cycle(ph))
			break ;
	}
	return (NULL);
}
