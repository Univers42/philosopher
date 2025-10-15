/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 20:48:11 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/15 17:09:45 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <pthread.h>
#include <stdarg.h>
#include <sys/time.h>

int	print_game(t_philo *phils, char *str)
{
	int	should_print;
	struct timeval		 tv;
	unsigned long long	 now_us;
	unsigned long long	 start_us;
	unsigned long long	 rel_us;
	unsigned long long	 rel_ms;
	unsigned long long	 frac_us;

	gettimeofday(&tv, NULL);
	now_us = (unsigned long long)tv.tv_sec * 1000000ULL + (unsigned long long) tv.tv_usec;
	start_us = (unsigned long long)phils->c->program_start_time * 1000ULL;
	if (now_us > start_us)
		rel_us = now_us - start_us;
	else
		rel_us = 0;
	rel_ms = rel_us / 1000ULL;
	frac_us = rel_us % 1000ULL;
	pthread_mutex_lock(&phils->c->printing);
	get_value_safe(&phils->c->m_finish, &should_print,
		&phils->c->finish, sizeof(int));
	should_print = (should_print == FALSE);
	if (should_print || ft_strlen(str) == ft_strlen(DIED))
	{
		printf("[%7llu.%03llu ms] [P:%3d] %s\n", rel_ms, frac_us, phils->id, str);
	}
	pthread_mutex_unlock(&phils->c->printing);
	return (FALSE);
}

// this is literally a copy pase of an old work
// it doens't comply the norminette
void	debug_print(t_data *c, int philo_id, const char *format, ...)
{
	va_list	args;

	if (!c->debug)
		return ;
	pthread_mutex_lock(&c->printing);
	printf("\033[1;35m[DEBUG] %ld ms - Philo %d: ",
		time_dif(c->program_start_time), philo_id);
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	printf("\033[0m\n");
	pthread_mutex_unlock(&c->printing);
}

void	print_one(t_time time)
{
	printf("[      0.000 ms] [P:  1] has taken a fork\n");
	printf("[%7ld.000 ms] [P:  1] %s\n", (long)ft_usleep(time), DIED);
}
