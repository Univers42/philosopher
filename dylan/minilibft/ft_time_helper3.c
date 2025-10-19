/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_time_helper3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 15:00:15 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 15:00:22 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "minilibft.h"
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

uint64_t	elapsed_ns(t_time start, t_time end)
{
	return (end - start);
}

double	ns_to_s(t_time ns)
{
	return ((double)ns / 1e9);
}

uint64_t	ts_to_ns(struct timespec ts)
{
	return (((uint64_t)ts.tv_sec * 1000000000ULL) + (uint64_t)ts.tv_nsec);
}

struct timespec	ns_to_ts(uint64_t ns)
{
	struct timespec	ts;

	ts.tv_sec = (time_t)(ns / 1000000000ULL);
	ts.tv_nsec = (long)(ns % 1000000000ULL);
	return (ts);
}

uint64_t	mono_now_ns(void)
{
	struct timespec	ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts_to_ns(ts));
}
