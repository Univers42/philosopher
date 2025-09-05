/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 12:47:08 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/05 12:56:19 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"






static init(int *values, t_philo *philos);

int main(int argc, char **argv)
{
	int     values[5];
	t_philo philo[];
	pthread_t thread_id[N];
	pthread_attr_t attr;
	int				i;

	ft_pthread_attr_init(&attr);
	ft_pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	i = 0;
	while (philo[i] < values[0])
		pthread_join(thread_id[i], NULL);
	ft_pthread_attr_destroy(&attr);
	ft_pthread_exit(NULL);
	argcheck(argc, argv, values);
	philo.init(values, philo);
	philo[philo]
	return (0);
}

static init(int *values, t_philo *philos)
{
	int num_philos;

	num_philos = values[0];
	for (int i = 0; i < num_philos; i++)
	{
		philos[i].id = i + 1;
		philos[i].time_to_die = values[1];
		philos[i].time_to_eat = values[2];
		philos[i].time_to_sleep = values[3];
		philos[i].state = THINKING;
		if (values[4] != -1)
			philos[i].meals_required = values[4];
		else
			philos[i].meals_required = -1;
		philos[i].left_fork = i;
		philos[i].right_fork = (i + 1) % num_philos;
		philos[i].last_meal_time = 0;
		philos[i].meals_eaten = 0;
		philos[i].thread = NULL;
	}
}
