/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 12:47:06 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/06 18:49:37 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>

int conv_pos_int(char *str)
{
	int result;
	int i;

	result = 0;
	while (*str == 32 || (*str >= 9 && *str <= 13))
		str++;
	i = 0;
	while (str[i])
	{
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (result);
}

void    argcheck(int argc, char **argv, int *values)
{
	int i, j;

	if (argc != 5 && argc != 6)
	{
		printf("Error: Invalid number of arguments.\n");
		exit(1);
	}
	i = 1;
	while (i < argc)
	{
		j = 0;
		while (argv[i][j])
		{
			if (argv[i][j] < '0' || argv[i][j] > '9')
			{
				printf("Error: Arguments must be positive integers.\n");
				exit(1);
			}
			j++;
		}
		values[i - 1] = conv_pos_int(argv[i]);
		i++;
	}
	if (argc == 5)
		values[4] = -1;
	if (values[0] <= 0 || values[1] <= 0 || values[2] <= 0 || values[3] <= 0)
	{
		printf("Error: Arguments must be > 0.\n");
		exit(1);
	}
}

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000L) + (tv.tv_usec / 1000L));
}

long	get_time_us(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000000L + tv.tv_usec);
}

int	sim_get_stop(t_sim *sim)
{
	int v;
	pthread_mutex_lock(&sim->state);
	v = sim->stop;
	pthread_mutex_unlock(&sim->state);
	return v;
}

void	sim_set_stop(t_sim *sim)
{
	pthread_mutex_lock(&sim->state);
	sim->stop = 1;
	pthread_mutex_unlock(&sim->state);
}

void	ft_usleep_ms(long ms, t_sim *sim)
{
	long	now_us;
	long	end_us;

	end_us = get_time_us() + (ms * 1000);
	while (!sim_get_stop(sim))
	{
		now_us = get_time_us();
		if (now_us >= end_us)
			break ;
		long remaining_us = end_us - now_us;
		if (remaining_us > 5000)
			usleep(2000);
		else if (remaining_us > 2000)
			usleep(1000);
		else if (remaining_us > 1000)
			usleep(500);
		else if (remaining_us > 200)
			usleep(100);
		else
			usleep(50);
	}
}

void	log_status(t_sim *sim, int id, const char *msg)
{
	long	now;

	pthread_mutex_lock(&sim->print);
	if (!sim->stop)
	{
		now = get_time_ms() - sim->start_ms;
		printf("%ld %d %s\n", now, id, msg);
	}
	pthread_mutex_unlock(&sim->print);
}

void	dbg(t_sim *sim, int id, const char *tag, const char *fmt, ...)
{
#ifdef DEBUG
	long		now;
	va_list		ap;

	pthread_mutex_lock(&sim->print);
	now = get_time_ms() - sim->start_ms;
	fprintf(stderr, "[DBG %ld][id:%d][%s] ", now, id, tag);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	pthread_mutex_unlock(&sim->print);
#else
	(void)sim; (void)id; (void)tag; (void)fmt;
#endif
}

void	death_timing_log(int id, long expected_ms, long actual_ms, long delta_ms)
{
#ifdef DEATH_CHECK
	fprintf(stderr,
		"[DEATH_CHECK] philo=%d expected=%ldms actual=%ldms delta=%ldms%s\n",
		id, expected_ms, actual_ms, delta_ms,
		(delta_ms > DEATH_TOLERANCE_MS ? "  (OVER TOLERANCE!)" : ""));
#else
	(void)id; (void)expected_ms; (void)actual_ms; (void)delta_ms;
#endif
}