/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 22:04:39 by syzygy            #+#    #+#             */
/*   Updated: 2025/10/03 13:14:44 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_ARGS 6
#define ATOL_ERROR -1
#define NO_OPT -1
#define USAGE_ARGS "number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]"

static inline int ft_is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

int positive_atol(char *ascii)
{
	long	res;
	int		digit;
	
	res = 0;
	if (!ascii || !*ascii)
		return (ATOL_ERROR);
	while (ft_is_digit(*ascii))
	{
		digit = (*ascii - '0');
		if (res > (__LONG_MAX__ - digit) / 10)
			return (ATOL_ERROR);
		res = res * 10 + digit;
		ascii++;		
	}
	if (*ascii != '\0')
		return (ATOL_ERROR);
	return (res);
}

int	convert_args(int argc, char **argv, long *values)
{
	int	i;

	i = 0;
	while (++i < argc)
	{
		values[i - 1] = positive_atol(argv[i]);
		if (values[i - 1] == ATOL_ERROR)
			return (0);
	}
	if (argc < 6)
		values[4] = NO_OPT;
	return (1);
}

int main(int argc, char **argv)
{
	int		i;
	long	values[MAX_ARGS];

	if (argc < 5 || argc > 6)
		printf("Usage: %s \n"USAGE_ARGS, argv[0]);
	if (!convert_args(argc, argv, values))
		return (1);
	
	printf("Parsed arguments:\n");
	printf("Number of philosophers: %ld\n", values[0]);
	printf("Time to die: %ld\n", values[1]);
	printf("Time to eat: %ld\n", values[2]);
	printf("Time to sleep: %ld\n", values[3]);
	if (values[4] != NO_OPT)
		printf("Number of times each must eat: %ld\n", values[4]);
	else
		printf("No limit on eating times\n");
	
	return (0);
}