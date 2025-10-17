/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils_1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvassall <mvassall@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/30 13:43:55 by mvassall          #+#    #+#             */
/*   Updated: 2025/07/11 16:15:24 by mvassall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int	ft_is_int(char *str)
{
	unsigned int	count_digits;

	if (str == NULL)
		return (0);
	while (*str && (*str == ' ' || *str == '\t'))
		str++;
	if (*str && (*str == '-' || *str == '+'))
		str++;
	count_digits = 0;
	while (*str && ('0' <= *str && *str <= '9'))
	{
		count_digits++;
		str++;
	}
	while (*str && (*str == ' ' || *str == '\t'))
		str++;
	if (*str != '\0')
		return (0);
	return (count_digits <= 10);
}

int	ft_atoi(const char *str)
{
	int	val;
	int	is_positive;

	while (*str && (*str == ' ' || *str == '\t'))
		str++;
	if (*str == '\0')
		return (0);
	is_positive = 1;
	if (*str == '+' || *str == '-')
	{
		is_positive = (*str != '-');
		str++;
	}
	val = 0;
	while (*str && ('0' <= *str && *str <= '9'))
		val = (val * 10) - (*str++ - '0');
	if (is_positive)
		return (-val);
	return (val);
}

int	get_args(int ac, char **av, int *values)
{
	if (ac != 5 && ac != 6)
		return (0);
	if (!ft_is_int(av[1]) || !ft_is_int(av[2]) || !ft_is_int(av[3])
		|| !ft_is_int(av[4]))
		return (0);
	if (ac == 6 && !ft_is_int(av[5]))
		return (0);
	memset(values, 0, 5 * sizeof(int));
	values[N_PHILOSOPHERS] = ft_atoi(av[1]);
	values[TIME_TO_DIE] = ft_atoi(av[2]);
	values[TIME_TO_EAT] = ft_atoi(av[3]);
	values[TIME_TO_SLEEP] = ft_atoi(av[4]);
	values[N_EATING_ROUNDS] = 0;
	if (ac == 6)
	{
		values[N_EATING_ROUNDS] = ft_atoi(av[5]);
		if (values[N_EATING_ROUNDS] < 0)
			return (0);
	}
	if (values[N_PHILOSOPHERS] < 1 || values[TIME_TO_DIE] <= 0
		|| values[TIME_TO_EAT] <= 0 || values[TIME_TO_SLEEP] <= 0)
		return (0);
	return (1);
}

uint64_t	get_time_us(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL))
		return (0);
	return ((tv.tv_sec * (uint64_t)1000000) + tv.tv_usec);
}

void	*ft_calloc(size_t n_members, size_t size)
{
	unsigned long	tsize;
	void			*ptr;

	tsize = n_members * size;
	ptr = malloc(tsize);
	if (ptr != NULL)
		memset(ptr, 0, tsize);
	return (ptr);
}
