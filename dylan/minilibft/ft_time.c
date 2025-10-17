/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_time.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 18:38:53 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/16 23:01:53 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include "minilibft.h"
#include <sys/time.h>
#include <unistd.h>

#define PRECISE_COARSE_NS 2000000ULL
#define PRECISE_MARGIN_NS 200000ULL
#define PRECISE_SPIN_NS   50000ULL

typedef struct s_precise_stats {
	double	estimate;
	double	mean;
	double	m2;
	uint64_t count;
}	t_precise_stats;

static t_precise_stats	g_precise = {5e-3, 5e-3, 0.0, 1};

static double	timeval_diff(struct timeval start, struct timeval end)
{
	double	sec;
	double	usec;

	sec = (double)(end.tv_sec - start.tv_sec);
	usec = (double)(end.tv_usec - start.tv_usec);
	return (sec + usec / 1000000.0);
}

static uint64_t	timeval_diff_ns(struct timeval start, struct timeval end)
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

static double	ft_sqrt(double x)
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

void	ft_usleep(unsigned long long time_in_ms)
{
	t_time 	start;
	t_time	current;

	start = get_time();
	while(1)
	{
		current = get_time();
		if (current - start >= time_in_ms)
			break ;
		if (time_in_ms - (current - start) > 10)
			usleep(5000);
		else
			usleep(100);
	}
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
		ft_precise_sleep(1000000ULL);
	}
	return (wait);
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

uint64_t	ft_precise_sleep(uint64_t duration_ns)
{
	struct timeval	begin;
	struct timeval	coarse_start;
	struct timeval	coarse_end;
	struct timeval	spin_start;
	struct timeval	now;
	double			remaining;
	double			observed;
	double			delta;
	double			variance;
	double			stddev;
	uint64_t		spin_ns;
	uint64_t		elapsed;

	if (duration_ns == 0)
		return (0);
	gettimeofday(&begin, NULL);
	remaining = (double)duration_ns / 1000000000.0;
	while (remaining > g_precise.estimate)
	{
		gettimeofday(&coarse_start, NULL);
		usleep(1000);
		gettimeofday(&coarse_end, NULL);
		observed = timeval_diff(coarse_start, coarse_end);
		remaining -= observed;
		if (remaining < 0.0)
			remaining = 0.0;
		g_precise.count++;
		delta = observed - g_precise.mean;
		g_precise.mean += delta / (double)g_precise.count;
		g_precise.m2 += delta * (observed - g_precise.mean);
		if (g_precise.count > 1)
		{
			variance = g_precise.m2 / (double)(g_precise.count - 1);
			if (variance < 0.0)
				variance = 0.0;
			stddev = ft_sqrt(variance);
		}
		else
			stddev = 0.0;
		g_precise.estimate = g_precise.mean + stddev;
		if (g_precise.estimate < 0.0005)
			g_precise.estimate = 0.0005;
		if (remaining <= 0.0)
			break ;
	}
	spin_ns = (uint64_t)(remaining * 1000000000.0);
	if (spin_ns > 0)
	{
		gettimeofday(&spin_start, NULL);
		while (TRUE)
		{
			gettimeofday(&now, NULL);
			elapsed = timeval_diff_ns(spin_start, now);
			if (elapsed >= spin_ns)
				break ;
			if (spin_ns - elapsed > 2000ULL)
				usleep(0);
		}
	}
	gettimeofday(&now, NULL);
	return (timeval_diff_ns(begin, now));
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