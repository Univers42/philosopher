/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_time.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 18:38:53 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/16 11:26:01 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "minilibft.h"
#include <sys/time.h>

t_time	cur_time(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + time.tv_usec / 1000);
}

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
t_time	ft_usleep(t_time wait)
{
	t_time	limit;

	limit = cur_time() + wait;
	while (TRUE)
	{
		if (cur_time() >= limit)
			break ;
		usleep(100);
	}
	return (wait);
}

t_time	time_dif(t_time since)
{
	t_time now = cur_time();
	return (now - since);
}

t_time	now_ns(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return ((uint64_t)tv.tv_sec * 1000000000ULL + (uint64_t)tv.tv_usec * 1000ULL);
}

t_time ft_precise_sleep(t_time duration_ns)
{
    uint64_t t0 = now_ns();
    uint64_t target = t0 + duration_ns;

    // Coarse sleep for most of the duration
    if (duration_ns > (PERIOD_NS + TOLERANCE_NS)) {
        uint64_t coarse = duration_ns - (PERIOD_NS + TOLERANCE_NS);
        uint64_t ms = coarse / 1000000ULL;  // convert ns → ms
        if (ms > 0)
            usleep(ms * 1000);              // convert ms → µs for usleep()
    }

    // Spin until the target time
    while (now_ns() < target) {
        // Optional: usleep(0) gives the CPU a tiny break
        usleep(0);
    }

    uint64_t t1 = now_ns();
    return t1 - t0; // return actual elapsed nanoseconds
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