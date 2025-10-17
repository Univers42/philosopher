/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glacroix <glacroix@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 19:23:22 by glacroix          #+#    #+#             */
/*   Updated: 2023/09/26 15:41:22 by glacroix         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_bonus.h"

int	ft_isdigit(char c)
{
	return (c >= '0' && c <= '9');
}

void	ft_sleep(long long time, t_philo *philo)
{
	long long	start;
	long long	test;

	start = current_time() + time;
	while (current_time() < start)
	{
		test = current_time() - philo->data->start_time
			- philo->finished_eating_time;
		if (philo->data->n_philos == 1)
			test = philo->data->time_to_die;
		if (test >= philo->data->time_to_die)
		{
			sem_wait(philo->data->sem_death);
			printf("%llu %d died\n", test, philo->id);
			exit(EXIT_DEATH);
		}
		usleep(500);
	}
}

int	is_number(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+')
		i++;
	while (str[i] != '\0')
	{
		if (!ft_isdigit(str[i++]))
			return (0);
	}
	return (1);
}

long long	current_time(void)
{
	struct timeval	time;
	long long		current_time;

	gettimeofday(&time, NULL);
	current_time = (time.tv_sec) * 1000 + (time.tv_usec) / 1000;
	return (current_time);
}

int	ft_atoi(char *str)
{
	int				i;
	int				signage;
	unsigned int	result;

	i = 0;
	result = 0;
	signage = 0;
	while ((str[i] > 8 && str[i] < 14) || str[i] == 32)
		i++;
	if (str[i] == 43 || str[i] == 45)
	{
		if (str[i++] == 45)
			signage++;
	}
	while (str[i] >= '0' && str[i] <= '9')
		result = ((str[i++] - '0') + (result * 10));
	if (result > 2147483647 && !signage)
		return (-1);
	else if (result > 2147483648 && signage)
		return (0);
	else if (signage)
		return ((int)result * -1);
	return ((int) result);
}
