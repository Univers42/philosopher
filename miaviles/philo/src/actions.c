/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 18:27:07 by miaviles          #+#    #+#             */
/*   Updated: 2025/10/16 17:25:30 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

void sleep_and_think(t_philo *philo)
{
	print_state(philo, "is sleeping");
	ft_usleep(philo->rules->time_to_sleep);
	print_state(philo, "is thinking");
	if (philo->rules->nb_philosophers % 2 == 1)
		ft_usleep(philo->rules->time_to_eat / 2);
	else if (philo->rules->nb_philosophers > 1)
		ft_usleep(1);
}

void release_forks(t_philo *philo)
{
	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_unlock(philo->left_fork);
}

int take_forks(t_philo *philo)
{
	if (philo->id % 2 == 1)
	{
		pthread_mutex_lock(philo->left_fork);
		print_state(philo, "has taken a fork");
		if (!get_simulation_state(philo->rules))
		{
			pthread_mutex_unlock(philo->left_fork);
			return (0);
		}
		pthread_mutex_lock(philo->right_fork);
		print_state(philo, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(philo->right_fork);
		print_state(philo, "has taken a fork");
		if (!get_simulation_state(philo->rules))
		{
			pthread_mutex_unlock(philo->right_fork);
			return (0);
		}
		pthread_mutex_lock(philo->left_fork);
		print_state(philo, "has taken a fork");
	}
	return (1);
}

void eat(t_philo *philo)
{
	pthread_mutex_lock(&philo->meal_lock);
	philo->last_meal = get_time();
	pthread_mutex_unlock(&philo->meal_lock);
	print_state(philo, "is eating");
	ft_usleep(philo->rules->time_to_eat);
	pthread_mutex_lock(&philo->meal_lock);
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->meal_lock);
}
