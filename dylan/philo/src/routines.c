/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routines.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 19:59:40 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/06 01:00:45 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/**
Here we will be doing the thread routines, not mistaken with threads
themselves

a thread routine or thread entry point is a function that execute an action
here for each philosopher we will monitoe different things..
this function run in parallel with others...
out thread will perform monitorizations and actions
*/

void	*death_checker(void *p)
{
	t_data *c;
	int		i;

	c = (t_data *)p;
	while (TRUE)
	{
		if (done(&c->philos[0]) == TRUE)
			return (NULL);
		i = 0;
		while (i < c->args[PHILO_C])
		{
			if (is_dead(&c->philos[i]) == FALSE)
				i++;
			else
				return (set_death_and_exit(c, i, PRINT));
		}
		usleep(100);
	}
	return (NULL);
}

void	*meals_checker(void *p)
{
	t_data *c;
	int		i;

	c = (t_data *)p;
	while (TRUE)
	{
		if (done(&c->philos[0]) == TRUE)
			return (NULL);
		i = 0;
		while (i < c->args[PHILO_C])
		{
			if (had_all_meals(c, i) == FALSE)
				break ;
			i++;
		}
		if (i == c->args[PHILO_C])
			return (set_death_and_exit(c, i, NO_PRINT));
		usleep(100);
	}
	return (NULL);
}

void	*dinner(void *p)
{
	t_philo	*philo;

	philo = (t_philo*)p;
	pthread_mutex_lock(&(philo->eating_mutex));
	philo->last_meal = philo->c->program_start_time;
	philo->max_time_to_eat = ft_positive_offset(philo->last_meal, philo->c->args[T_DIE]);
	pthread_mutex_unlock(&(philo->eating_mutex));
	if (philo->id % 2 == 0)
		usleep(500);
	while (done(philo) == FALSE)
	{
		if (!get_fork(philo))
			continue; // did not acquire both forks (likely finish set), retry/loop
		eat(philo);
		leave_fork(philo);
	}
	return (NULL);	
}