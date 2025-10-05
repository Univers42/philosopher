/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 23:09:04 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/06 01:00:45 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>

// internal cancellable sleep based on finish flag
static void	sleep_cancellable(t_philo *p, t_time wait)
{
	t_time	end;

	end = ft_positive_offset(cur_time(), wait);
	while (done(p) == FALSE && cur_time() < end)
		usleep(1000);
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
	return (1);
}

void	eat(t_philo *p)
{
	int false_val = FALSE;

	if (done(p) == TRUE)
		return;
	pthread_mutex_lock(&(p->eating_mutex));
	p->is_eating = true;
	p->last_meal = cur_time();
	p->max_time_to_eat = ft_positive_offset(p->last_meal, p->c->args[T_DIE]);
	p->eat_count++;
	pthread_mutex_unlock(&(p->eating_mutex));
	print_game(p, EATING);
	sleep_cancellable(p, p->c->args[T_EAT]);
	set_value_safe(&p->eating_mutex, &p->is_eating, &false_val, sizeof(int));
}

void	leave_fork(t_philo *p)
{
	int first;
	int	second;

	print_game(p, SLEEPING);
	first = p->l_fork;
	second = p->r_fork;
	if (first > second)
		ft_swap(&first, &second, sizeof(int));
	pthread_mutex_unlock(&p->c->forks[second]);
	pthread_mutex_unlock(&p->c->forks[first]);
	unlock(p);
	sleep_cancellable(p, p->c->args[T_SLEEP]);
	print_game(p, THINKING);
}