/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glacroix <glacroix@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 18:13:29 by glacroix          #+#    #+#             */
/*   Updated: 2023/09/21 17:15:12 by glacroix         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

static void	logs(t_philo *philo, const char *str)
{
	pthread_mutex_lock(&philo->data->log_mutex);
	if (philo->data->log == FALSE)
	{
		pthread_mutex_unlock(&philo->data->log_mutex);
		return ;
	}
	else
		printf("%lld %d %s\n", current_time()
			- philo->data->start_time, philo->id, str);
	pthread_mutex_unlock(&philo->data->log_mutex);
}

void	takeforks(t_philo *philo)
{
	int left;
	int right;
	int first;
	int second;

	left = philo->id - 1;
	right = philo->id % philo->data->nbr_philos;
	/* lock in a fixed global order to avoid lock-order violations */
	first = (left < right) ? left : right;
	second = (left < right) ? right : left;

	pthread_mutex_lock(&philo->data->forks[first]);
	logs(philo, "has taken a fork");
	pthread_mutex_lock(&philo->data->forks[second]);
	logs(philo, "has taken a fork");

	pthread_mutex_lock(&philo->data->finished_eating_mutex);
	philo->finished_eating_time = current_time() - philo->data->start_time;
	pthread_mutex_unlock(&philo->data->finished_eating_mutex);
}

void	eat(t_philo *philo)
{
	if (philo->data->nbr_philos == 1)
		return ;
	logs(philo, "is eating");
	ft_sleep(philo->data->time_to_eat);
	philo->ate_count++;
}

void	drop_n_sleep(t_philo *philo)
{
	int left;
	int right;
	int first;
	int second;

	left = philo->id - 1;
	right = philo->id % philo->data->nbr_philos;
	/* if only one philosopher, routine() handles it; normal path always has two different forks */
	first = (left < right) ? left : right;
	second = (left < right) ? right : left;

	/* unlock in reverse order of locking */
	pthread_mutex_unlock(&philo->data->forks[second]);
	pthread_mutex_unlock(&philo->data->forks[first]);

	if (philo->data->max_eating_cycles > -1
		&& philo->ate_count >= philo->data->max_eating_cycles)
	{
		pthread_mutex_lock(&philo->data->max_eat_mutex);
		philo->data->exit_flag++;
		pthread_mutex_unlock(&philo->data->max_eat_mutex);
	}
	logs(philo, "is sleeping");
	ft_sleep(philo->data->time_to_sleep);
	logs(philo, "is thinking");
}

void	*routine(t_philo *philo)
{
	int	stop;

	stop = 0;
	pthread_mutex_lock(&philo->data->ready_set);
	pthread_mutex_unlock(&philo->data->ready_set);

	/* single philosopher: take one fork, wait, and exit cleanly */
	if (philo->data->nbr_philos == 1)
	{
		pthread_mutex_lock(&philo->data->forks[philo->id - 1]);
		logs(philo, "has taken a fork");
		ft_sleep(philo->data->time_to_die);
		pthread_mutex_unlock(&philo->data->forks[philo->id - 1]);
		return (NULL);
	}

	if (philo->id % 2 == 0)
		ft_sleep(50);
	while (!stop)
	{
		philo->start_time = current_time() - philo->data->start_time;
		if (philo->data->max_eating_cycles == 0)
			stop = 1;
		takeforks(philo);
		eat(philo);
		drop_n_sleep(philo);
		routine_stop(philo, &stop);
	}
	return (NULL);
}
