/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 23:09:04 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/15 17:02:29 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>

// internal cancellable sleep based on finish flag
static void	sleep_until_ms(t_philo *p, t_time deadline_ms)
{
	unsigned long long	end_us;
	unsigned long long	now;
	unsigned long long	rem;
	struct timeval		tv, tv2;
	
	end_us = (unsigned long long)deadline_ms * 1000ULL;
	while (done(p) == FALSE)
	{
		gettimeofday(&tv, NULL);
		now = (unsigned long long)tv.tv_sec * 1000000ULL + (unsigned long long)tv.tv_usec;
		if (now >= end_us)
			break;
		rem = end_us - now;
		if (rem > 2000ULL)
			usleep((useconds_t)(rem - 1000ULL));
		else if (rem > 200ULL)
			usleep(100);
		else
		{
			while (done(p) == FALSE)
			{
				gettimeofday(&tv2, NULL);
				if ((unsigned long long)tv2.tv_sec * 1000000ULL + (unsigned long long)tv2.tv_usec >= end_us)
					return;
			}
			return ;
		}
	}
}

// limit contenders to N-1 to prevent starvation with global fork order
int	lock(t_philo *p)
{
	while (TRUE)
	{
		if (done(p) == TRUE)
			return (0);
		pthread_mutex_lock(&p->c->waiter);
		if (p->c->waiter_slots > 0)
		{
			p->c->waiter_slots--;
			pthread_mutex_unlock(&p->c->waiter);
			return (1);
		}
		pthread_mutex_unlock(&p->c->waiter);
		usleep(100);
	}
}

void unlock(t_philo *p)
{
	pthread_mutex_lock(&p->c->waiter);
	p->c->waiter_slots++;
	if (p->c->waiter_slots > p->c->args[PHILO_C] - 1)
		p->c->waiter_slots = p->c->args[PHILO_C] - 1; // clamp to N-1
	pthread_mutex_unlock(&p->c->waiter);
}

int	get_fork(t_philo *p)
{
	int	first;
	int	second;

	if (!lock(p))
		return (0);
	if (done(p) == TRUE)
	{
		unlock(p);
		return (0);
	}
	first = p->l_fork;
	second = p->r_fork;
	if (first > second)
		ft_swap(&first, &second, sizeof(int));
	// lock first fork
	pthread_mutex_lock(&p->c->forks[first]);
	if (done(p) == TRUE)
	{
		pthread_mutex_unlock(&p->c->forks[first]);
		unlock(p);
		return (0);
	}
	print_game(p, FORK_TAKEN);
	// lock second fork
	pthread_mutex_lock(&p->c->forks[second]);
	if (done(p) == TRUE)
	{
		pthread_mutex_unlock(&p->c->forks[second]);
		pthread_mutex_unlock(&p->c->forks[first]);
		unlock(p);
		return (0);
	}
	print_game(p, FORK_TAKEN);
	pthread_mutex_lock(&(p->eating_mutex));
	p->is_eating = TRUE;
	p->last_meal = cur_time();
	p->max_time_to_eat = ft_positive_offset(p->last_meal, p->c->args[T_DIE]);
	pthread_mutex_unlock(&(p->eating_mutex));
	return (1);
}

void	eat(t_philo *p)
{
	int			false_val = FALSE;
	t_time		eat_deadline;

	if (done(p) == TRUE)
		return;
	pthread_mutex_lock(&(p->eating_mutex));
	p->is_eating = true;
	p->last_meal = cur_time();
	eat_deadline = ft_positive_offset(p->last_meal, p->c->args[T_EAT]);
	p->max_time_to_eat = ft_positive_offset(p->last_meal, p->c->args[T_DIE]);
	p->eat_count++;
	pthread_mutex_unlock(&(p->eating_mutex));
	print_game(p, EATING);
	sleep_until_ms(p, eat_deadline);
	set_value_safe(&p->eating_mutex, &p->is_eating, &false_val, sizeof(int));
}

void	leave_fork(t_philo *p)
{
	int	first;
	int	second;
	t_time	base_meal;
	t_time	sleep_deadline;

	first = p->l_fork;
	second = p->r_fork;
	if (first > second)
		ft_swap(&first, &second, sizeof(int));
	pthread_mutex_unlock(&p->c->forks[second]);
	pthread_mutex_unlock(&p->c->forks[first]);
	unlock(p);
	print_game(p, SLEEPING);
	pthread_mutex_lock(&(p->eating_mutex));
	base_meal = p->last_meal;
	pthread_mutex_unlock(&(p->eating_mutex));
	sleep_deadline = ft_positive_offset(base_meal, (t_time)(p->c->args[T_EAT] + p->c->args[T_SLEEP]));
	sleep_until_ms(p, sleep_deadline);
	print_game(p, THINKING);
	
}