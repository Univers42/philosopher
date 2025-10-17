/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_time.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 18:38:53 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/17 20:31:28 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "minilibft.h"
#include <sys/time.h>
#include <unistd.h>
#include <time.h>

#define PRECISE_COARSE_NS 2000000ULL
#define PRECISE_MARGIN_NS 200000ULL
#define PRECISE_SPIN_NS   50000ULL

typedef struct s_precise_stats {
	double	estimate;
	double	mean;
	double	m2;
	uint64_t count;
}	t_precise_stats;

t_precise_stats	g_precise = {5e-3, 5e-3, 0.0, 1};

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
	return ((uint64_t)sec * 1000000000ULL) + ((uint64_t)usec * 1000ULL);
}

double	ft_sqrt(double x)
{
	double	guess;
	double	prev;
	int		i;

	if (x <= 0.0)
		return (0.0);
	guess = x;
	prev = 0.0;
	i = 0;
	while (i < 12 && guess != prev)
	{
		prev = guess;
		guess = 0.5 * (guess + x / guess);
		i++;
	}
	return (guess);
}

t_time	get_time(void)
{
	struct timeval	tv;
	unsigned long	ms;
	gettimeofday(&tv, NULL);
	ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	return (ms);
}

uint64_t elapsed_time(uint64_t time)
{
    return (cur_time() - time);
}
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

// forward declaration to use precise sleep in ft_usleep
t_time ft_precise_usleep(t_time wait);

//! may overlap, to use more precise sleep after that
// the usleep function is not guaranteed to be precise, and sometimes
// the actual sleep will be longer than expected
void	ft_usleep(unsigned long time_in_ms)
{
	ft_precise_usleep(time_in_ms);
}

t_time	time_dif(t_time since)
{
	t_time now = cur_time();
	return (now - since);
}

uint64_t	now_ns(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((uint64_t)tv.tv_sec * 1000000000ULL
		+ (uint64_t)tv.tv_usec * 1000ULL);
}

uint64_t	elapsed_ns(t_time start, t_time end)
{
	return (end - start);
}

double	ns_to_s(t_time ns)
{
	return ((double)ns / 1e9);
}

static inline uint64_t ts_to_ns(struct timespec ts)
{
	return ((uint64_t)ts.tv_sec * 1000000000ULL) + (uint64_t)ts.tv_nsec;
}

static inline struct timespec ns_to_ts(uint64_t ns)
{
	struct timespec ts;
	ts.tv_sec = (time_t)(ns / 1000000000ULL);
	ts.tv_nsec = (long)(ns % 1000000000ULL);
	return (ts);
}

static inline uint64_t mono_now_ns(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts_to_ns(ts));
}

uint64_t	ft_precise_sleep(uint64_t duration_ns)
{
	uint64_t	start_ns;
	uint64_t	deadline_ns;
	uint64_t	now_ns;
	uint64_t	remaining;

	if (duration_ns == 0)
		return (0);
	start_ns = mono_now_ns();
	deadline_ns = start_ns + duration_ns;
	while (1)
	{
		now_ns = mono_now_ns();
		if (now_ns >= deadline_ns)
			break ;
		remaining = deadline_ns - now_ns;
		if (remaining > 2ULL * 1000000ULL)
		{
			struct timespec req = ns_to_ts(remaining - 1000000ULL);
			nanosleep(&req, NULL);
		}
		else if (remaining > 50ULL * 1000ULL)
		{
			/* Let the OS breathe a bit more here */
			struct timespec req = ns_to_ts(remaining / 2);
			nanosleep(&req, NULL);
		}
		else
		{
			/* Instead of tight spinning, yield occasionally */
			while (mono_now_ns() < deadline_ns)
				usleep(50); // yield 50µs per loop
			break ;
		}
	}
	return (mono_now_ns() - start_ns);
}

// Sleep until an absolute deadline in ms (monotonic), minimizing drift across cycles
t_time	ft_sleep_until_ms(t_time deadline_ms)
{
	while (1)
	{
		t_time now_ms = cur_time();
		if (now_ms >= deadline_ms)
			break ;
		uint64_t remain_ns = (uint64_t)(deadline_ms - now_ms) * 1000000ULL;
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