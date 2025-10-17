/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glacroix <glacroix@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/14 18:44:27 by glacroix          #+#    #+#             */
/*   Updated: 2023/09/06 13:19:10 by glacroix         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

static int	init_mutex(t_data *data)
{
	int	i;

	i = -1;
	if (pthread_mutex_init(&data->ready_set, NULL) != 0)
		return (ft_putstr_fd("Error initializing mutex\n", 2), 1);
	if (pthread_mutex_init(&data->log_mutex, NULL) != 0)
		return (ft_putstr_fd("Error initializing mutex\n", 2), 1);
	if (pthread_mutex_init(&data->death_mutex, NULL) != 0)
		return (ft_putstr_fd("Error initializing mutex\n", 2), 1);
	if (pthread_mutex_init(&data->finished_eating_mutex, NULL) != 0)
		return (ft_putstr_fd("Error initializing mutex\n", 2), 1);
	if (pthread_mutex_init(&data->max_eat_mutex, NULL) != 0)
		return (ft_putstr_fd("Error initializing mutex\n", 2), 1);
	if (pthread_mutex_init(&data->init_mutex, NULL) != 0)
		return (ft_putstr_fd("Error initializing mutex\n", 2), 1);
	data->forks = malloc(sizeof(*data->forks) * data->nbr_philos);
	if (!data->forks)
		return (ft_putstr_fd("failed malloc data->forks\n", 2), 1);
	while (++i < data->nbr_philos)
		pthread_mutex_init(&data->forks[i], NULL);
	return (0);
}

int	init_args(int argc, char **argv, t_data *data)
{
	data->nbr_philos = ft_atoi(argv[1]);
	data->time_to_die = ft_atoi(argv[2]);
	data->time_to_eat = ft_atoi(argv[3]);
	data->time_to_sleep = ft_atoi(argv[4]);
	if (data_wrong(data) == TRUE)
		return (1);
	data->philo_died = FALSE;
	data->log = TRUE;
	data->exit_flag = 0;
	if (argc == 6)
		data->max_eating_cycles = ft_atoi(argv[5]);
	else
		data->max_eating_cycles = -1;
	if (init_mutex(data) == 1)
		return (1);
	return (0);
}

static int	init_philo(t_data *data, t_philo *philo, int *i)
{
	pthread_mutex_lock(&data->init_mutex);
	philo->id = *i + 1;
	philo->data = data;
	philo->ate_count = 0;
	philo->ate_enough = FALSE;
	philo->finished_eating_time = 0;
	if (pthread_create(&philo->thread, NULL, (void *) routine, philo) != 0)
	{
		pthread_mutex_unlock(&data->init_mutex);
		return (ft_putstr_fd("Error when creating thread\n", 2), 3);
	}
	pthread_mutex_unlock(&data->init_mutex);
	return (0);
}

int	init_threads(t_data *data)
{
	t_philo	*philo;
	int		i;
	int		stop;

	i = -1;
	stop = 0;
	philo = malloc(sizeof(*philo) * data->nbr_philos);
	if (!philo)
	{
		free(data->forks);
		return (ft_putstr_fd("Error failed malloc philo\n", 2), 4);
	}
	pthread_mutex_lock(&data->ready_set);
	while (++i < data->nbr_philos)
		stop = init_philo(data, philo + i, &i);
	data->start_time = current_time();
	pthread_mutex_unlock(&data->ready_set);
	while (!stop)
		routine_check(data, philo, &stop);
	thread_join(data, philo);
	free_memory(data, philo);
	return (0);
}

/*
1 800 200 200
a philo should die
4 310 200 100
a philo should die
4 200 205 200
a philo should die
5 800 200 200 7
no one should die, simulation should stop after 7 eats
4 410 200 200 10
no one should die, simulation should stop after 10 eats
-5 600 200 200
should error and not run (no crashing)
4 -5 200 200
should error and not run (no crashing)
4 600 -5 200
should error and not run (no crashing)
4 600 200 -5
should error and not run (no crashing)
4 600 200 200 -5
should error and not run (no crashing)
//--------------------------------------------
5 800 200 200
no one should die
5 600 150 150
no one should die
4 410 200 200
no one should die
100 800 200 200
no one should die
105 800 200 200
no one should die
200 800 200 200
no one should die



*/
*/
