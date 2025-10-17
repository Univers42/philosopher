/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glacroix <glacroix@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/14 19:32:04 by glacroix          #+#    #+#             */
/*   Updated: 2023/09/21 17:14:26 by glacroix         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

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

int	data_wrong(t_data *data)
{
	if (data->nbr_philos <= 0)
		return (1);
	if (data->time_to_die <= 0)
		return (1);
	if (data->time_to_eat <= 0)
		return (1);
	if (data->time_to_sleep <= 0)
		return (1);
	return (0);
}

int	philo_died(t_philo *philo)
{
	long long	last_eat;

	/* quick check under death_mutex */
	pthread_mutex_lock(&philo->data->death_mutex);
	if (philo->data->philo_died == TRUE)
	{
		pthread_mutex_unlock(&philo->data->death_mutex);
		return (1);
	}
	pthread_mutex_unlock(&philo->data->death_mutex);

	/* read last eat time under its own mutex */
	pthread_mutex_lock(&philo->data->finished_eating_mutex);
	last_eat = philo->finished_eating_time;
	pthread_mutex_unlock(&philo->data->finished_eating_mutex);
	if (current_time() - philo->data->start_time - last_eat >= philo->data->time_to_die)
	{
		pthread_mutex_lock(&philo->data->death_mutex);
		philo->data->philo_died = TRUE;
		pthread_mutex_unlock(&philo->data->death_mutex);
		pthread_mutex_lock(&philo->data->log_mutex);
		philo->data->log = FALSE;
		pthread_mutex_unlock(&philo->data->log_mutex);
		return (1);
	}
	return (0);
}

void	routine_stop(t_philo *philo, int *stop)
{
	pthread_mutex_lock(&philo->data->death_mutex);
	if (philo->data->philo_died == TRUE)
		*stop = 1;
	pthread_mutex_unlock(&philo->data->death_mutex);

	pthread_mutex_lock(&philo->data->max_eat_mutex);
	if (philo->data->exit_flag >= philo->data->nbr_philos)
		*stop = 1;
	pthread_mutex_unlock(&philo->data->max_eat_mutex);
}

void	routine_check(t_data *data, t_philo *philo, int *stop)
{
	int	i;
	int	died;

	i = -1;
	while (++i < data->nbr_philos)
	{
		pthread_mutex_lock(&philo->data->max_eat_mutex);
		if (data->exit_flag >= data->nbr_philos)
		{
			pthread_mutex_unlock(&philo->data->max_eat_mutex);
			*stop = 1;
			break ;
		}
		pthread_mutex_unlock(&philo->data->max_eat_mutex);

		philo_died(&philo[i]);

		pthread_mutex_lock(&data->death_mutex);
		died = data->philo_died;
		pthread_mutex_unlock(&data->death_mutex);

		if (died == 1 && data->exit_flag != data->nbr_philos)
		{
			printf("%llu %d died\n", current_time() - data->start_time, philo[i].id);
			*stop = 1;
			pthread_mutex_lock(&philo->data->log_mutex);
			data->log = FALSE;
			pthread_mutex_unlock(&philo->data->log_mutex);
			break ;
		}
	}
}
