/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_time_helper4.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 15:10:00 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 15:10:25 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minilibft.h"
#include <time.h>
#include <unistd.h>

static void	sleep_coarse(uint64_t remaining)
{
	struct timespec	req;

	req = ns_to_ts(remaining - 1000000ULL);
	nanosleep(&req, NULL);
}

static void	sleep_medium(uint64_t remaining)
{
	struct timespec	req;

	req = ns_to_ts(remaining / 2);
	nanosleep(&req, NULL);
}

uint64_t	ft_precise_sleep_impl(uint64_t duration_ns)
{
	uint64_t	start_ns;
	uint64_t	deadline_ns;
	uint64_t	now_ns;
	uint64_t	remaining;

	start_ns = mono_now_ns();
	deadline_ns = start_ns + duration_ns;
	while (1)
	{
		now_ns = mono_now_ns();
		if (now_ns >= deadline_ns)
			break ;
		remaining = deadline_ns - now_ns;
		if (remaining > 2ULL * 1000000ULL)
			sleep_coarse(remaining);
		else if (remaining > 50ULL * 1000ULL)
			sleep_medium(remaining);
		else
		{
			while (mono_now_ns() < deadline_ns)
				usleep(50);
			break ;
		}
	}
	return (mono_now_ns() - start_ns);
}
