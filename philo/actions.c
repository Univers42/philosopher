/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 23:35:10 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 17:10:40 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	sleep_and_think(t_philo *philo)
{
	print_state(philo, "is sleeping");
	ft_precise_usleep(philo->ctx->args[TT_SLEEP]);
	print_state(philo, "is thinking");
	if (philo->ctx->n_philo % 2 == 1)
		ft_precise_usleep(philo->ctx->args[TT_EAT] / 2);
	else if (philo->ctx->n_philo > 1)
		ft_precise_usleep(1);
}

void	release_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_unlock(philo->left_fork);
}

static int	other_fork(t_philo *philo)
{
	pthread_mutex_lock(philo->right_fork);
	print_state(philo, "has taken a fork");
	if (DEBUG_MODE)
		log_debug(philo, LOG_INFO, "Acquired right fork");
	if (!get_simulation_state(philo->ctx))
		return (pthread_mutex_unlock(philo->right_fork), 0);
	pthread_mutex_lock(philo->left_fork);
	print_state(philo, "has taken a fork");
	if (DEBUG_MODE)
		log_debug(philo, LOG_INFO, "Acquired left fork");
	if (!get_simulation_state(philo->ctx))
	{
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);
		return (0);
	}
	return (1);
}

int	take_forks(t_philo *philo)
{
	if (philo->id % 2 == 1)
	{
		pthread_mutex_lock(philo->left_fork);
		print_state(philo, "has taken a fork");
		if (DEBUG_MODE)
			log_debug(philo, LOG_INFO, "Acquired left fork");
		if (!get_simulation_state(philo->ctx))
			return (pthread_mutex_unlock(philo->left_fork), 0);
		pthread_mutex_lock(philo->right_fork);
		print_state(philo, "has taken a fork");
		if (DEBUG_MODE)
			log_debug(philo, LOG_INFO, "Acquired right fork");
		if (!get_simulation_state(philo->ctx))
		{
			pthread_mutex_unlock(philo->right_fork);
			pthread_mutex_unlock(philo->left_fork);
			return (0);
		}
	}
	else
		if (!other_fork(philo))
			return (0);
	return (1);
}

void	eat(t_philo *philo)
{
	pthread_mutex_lock(&philo->meal_lock);
	philo->last_meal = get_time();
	pthread_mutex_unlock(&philo->meal_lock);
	print_state(philo, "is eating");
	ft_precise_usleep(philo->ctx->args[TT_EAT]);
	pthread_mutex_lock(&philo->meal_lock);
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->meal_lock);
}
