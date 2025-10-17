/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   base_setup.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glacroix <glacroix@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 19:21:42 by glacroix          #+#    #+#             */
/*   Updated: 2023/09/21 18:06:54 by glacroix         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_bonus.h"

int	args_wrong(int argc, char **argv)
{
	int	i;

	i = 0;
	if (argc < 5 || argc > 6)
		return (1);
	while (argv[++i] != NULL)
	{
		if (is_number(argv[i]) == FALSE)
			return (1);
	}
	return (0);
}

int	init_args(int argc, char **argv, t_data *data)
{
	data->n_philos = ft_atoi(argv[1]);
	data->time_to_die = ft_atoi(argv[2]);
	data->time_to_eat = ft_atoi(argv[3]);
	data->time_to_sleep = ft_atoi(argv[4]);
	if (data_wrong(data) == TRUE)
		return (1);
	if (argc == 6)
		data->max_eating_cycles = ft_atoi(argv[5]);
	else
		data->max_eating_cycles = -1;
	if (init_semaphore(data) == 1)
		return (1);
	return (0);
}

int	data_wrong(t_data *data)
{
	if (data->n_philos <= 0)
		return (1);
	if (data->time_to_die <= 0)
		return (1);
	if (data->time_to_eat <= 0)
		return (1);
	if (data->time_to_sleep <= 0)
		return (1);
	return (0);
}

int	init_semaphore(t_data *data)
{
	sem_unlink("/forks");
	sem_unlink("/print");
	sem_unlink("/start_time");
	sem_unlink("/data");
	sem_unlink("/death");
	data->sem_death = sem_open("/death", O_CREAT, 0644, 1);
	data->sem_print = sem_open("/print", O_CREAT, 0644, 1);
	data->sem_start_time = sem_open("/start_time", O_CREAT, 0644, 1);
	data->sem_data = sem_open("/data", O_CREAT, 0644, 1);
	data->forks = sem_open("/forks", O_CREAT, 0644, data->n_philos);
	if (data->forks == SEM_FAILED || data->sem_data == SEM_FAILED
		|| data->sem_death == SEM_FAILED
		|| data->sem_start_time == SEM_FAILED
		|| data->sem_print == SEM_FAILED)
		return (ft_putstr_fd("Error when creating the semaphore\n", 2), 1);
	return (0);
}
