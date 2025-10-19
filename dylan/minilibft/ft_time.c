/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_time.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 18:38:53 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 15:12:42 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "minilibft.h"
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

uint64_t	ft_precise_sleep(uint64_t duration_ns)
{
	if (duration_ns == 0)
		return (0);
	return (ft_precise_sleep_impl(duration_ns));
}

// Sleep until an absolute deadline in ms (monotonic),
// minimizing drift across cycles
t_time	ft_sleep_until_ms(t_time deadline_ms)
{
	t_time		now_ms;
	uint64_t	remain_ns;

	while (1)
	{
		now_ms = cur_time();
		if (now_ms >= deadline_ms)
			break ;
		remain_ns = (uint64_t)(deadline_ms - now_ms) * 1000000ULL;
		ft_precise_sleep(remain_ns);
	}
	return (deadline_ms);
}

t_time	ft_precise_usleep(t_time wait)
{
	ft_precise_sleep(wait * 1000000ULL);
	return (wait);
}

//int	main(int argc, char **argv)
//{
//	(void)argc;
//	(void)argv;
//
//	t_time start = cur_time();
//	printf("Current time (ms): %llu\n", (unsigned long long)start);
//
//	t_time offset = 500; // 500 ms
//	t_time future = ft_positive_offset(start, offset);
//	printf("Future time (+500ms): %llu\n", (unsigned long long)future);
//
//	t_time elapsed = ft_neg_offset(start);
//	printf("Elapsed since start (ms): %llu\n", (unsigned long long)elapsed);
//
//	printf("Sleeping for 500ms using ft_usleep...\n");
//	t_time before = cur_time();
//	ft_usleep(500); // sleep for 500 ms
//	t_time after = cur_time();
//	printf("Slept for: %llu ms (should be ~500ms)\n",
// 		(unsigned long long)(after - before));
//
//	// Demonstrate imprecision/overlap
//	printf("Demonstrating usleep imprecision:\n");
//	for (int i = 0; i < 5; ++i) {
//		t_time t0 = cur_time();
//		usleep(1000); // request 1ms
//		t_time t1 = cur_time();
//		printf("Requested 1ms, actual: %llu ms\n",
// 		(unsigned long long)(t1 - t0));
//	}
//
//	return 0;
//}