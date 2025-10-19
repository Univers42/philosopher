/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_time_helper1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 14:58:05 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 15:00:39 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "minilibft.h"
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

t_time	ft_positive_offset(t_time start, t_time offset)
{
	return (start + offset);
}

t_time	ft_neg_offset(t_time offset)
{
	return (cur_time() - offset);
}

//! may overlap, to use more precise sleep after that
// the usleep function is not guaranteed to be precise, and sometimes
// the actual sleep will be longer than expected
void	ft_usleep(unsigned long time_in_ms)
{
	ft_precise_sleep(time_in_ms * 1000000ULL);
}

t_time	time_dif(t_time since)
{
	t_time	now;

	now = cur_time();
	return (now - since);
}

uint64_t	now_ns(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((uint64_t)tv.tv_sec * 1000000000ULL
		+ (uint64_t)tv.tv_usec * 1000ULL);
}
