/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glacroix <glacroix@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/14 18:57:55 by glacroix          #+#    #+#             */
/*   Updated: 2023/09/15 18:45:00 by glacroix         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

int	ft_isdigit(char c)
{
	return (c >= '0' && c <= '9');
}

void	ft_sleep(long long time)
{
	long long	start;

	start = current_time() + time;
	while (current_time() < start)
		usleep(100);
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
