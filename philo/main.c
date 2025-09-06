/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/06 18:52:40 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/06 18:54:28 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/philosopher.h"

void	philo_init(int *values, t_philo *philos, t_sim *sim)
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
