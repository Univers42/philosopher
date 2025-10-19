/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_time_helper2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 14:57:39 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 15:10:26 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "minilibft.h"
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

double	timeval_diff(struct timeval start, struct timeval end)
{
	double	sec;
	double	usec;

	sec = (double)(end.tv_sec - start.tv_sec);
	usec = (double)(end.tv_usec - start.tv_usec);
	return (sec + usec / 1000000.0);
}

uint64_t	timeval_diff_ns(struct timeval start, struct timeval end)
{
	long	sec;
	long	usec;

	sec = end.tv_sec - start.tv_sec;
	usec = end.tv_usec - start.tv_usec;
	if (usec < 0)
	{
		usec += 1000000;
		sec -= 1;
	}
	if (sec < 0)
		return (0);
	return (((uint64_t)sec * 1000000000ULL) + ((uint64_t)usec * 1000ULL));
}

t_time	get_time(void)
{
	struct timeval	tv;
	unsigned long	ms;

	gettimeofday(&tv, NULL);
	ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	return (ms);
}

uint64_t	elapsed_time(uint64_t time)
{
	return (cur_time() - time);
}

t_time	cur_time(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + time.tv_usec / 1000);
}
