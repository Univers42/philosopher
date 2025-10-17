/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 23:09:04 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/16 12:04:25 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <limits.h>
#include <errno.h>

// internal cancellable sleep based on finish flag
#define WAIT_CHUNK_NS 200000ULL  // 0.2 ms for tighter responsiveness
// Short bound while waiting for the second fork (ns)
#define SECOND_FORK_WAIT_NS 800000ULL  // 0.8 ms budget per attempt
#define SECOND_FORK_STEP_NS  50000ULL  // 0.05 ms polling step

static void	sleep_until_ms(t_philo *p, t_time deadline_ms)
{
	uint64_t	target_ns;
	uint64_t	now;
	uint64_t	remaining;
	uint64_t	chunk;

	target_ns = (uint64_t)deadline_ms * 1000000ULL;
	while (done(p) == FALSE)
	{
		now = now_ns();
		if (now >= target_ns)
			break ;
		remaining = target_ns - now;
		chunk = (remaining > WAIT_CHUNK_NS) ? WAIT_CHUNK_NS : remaining;
		ft_precise_sleep(chunk);
	}
}

static int	min_eat_count(t_philo *p)
{
	int	i;
	int	min;
	int	count;

	min = INT_MAX;
	i = 0;
	while (i < p->c->args[PHILO_C])
	{
		get_value_safe(&p->c->philos[i].eating_mutex, &count,
			&p->c->philos[i].eat_count, sizeof(int));
		if (count < min)
			min = count;
		i++;
	}
	if (min == INT_MAX)
		return (0);
	return (min);
}

static t_time	min_time_to_die_left(t_philo *p, t_time now)
{
	t_time	min;
	t_time	left;
	int		is_eating;
	int		i;

	min = UINT64_MAX;
	i = 0;
	while (i < p->c->args[PHILO_C])
	{
		pthread_mutex_lock(&p->c->philos[i].eating_mutex);
		is_eating = p->c->philos[i].is_eating;
		// cast long -> t_time before comparing/subtracting
		{
			t_time max_at = (t_time)p->c->philos[i].max_time_to_eat;
			left = (max_at > now) ? (max_at - now) : 0;
		}
		pthread_mutex_unlock(&p->c->philos[i].eating_mutex);
		if (is_eating == TRUE)
		{
			i++;
			continue;
		}
		if (left < min)
			min = left;
		i++;
	}
	return (min == UINT64_MAX) ? 0 : min;
}

static int	can_eat_now(t_philo *p)
{
	t_time	now;
	t_time	own_left;
	t_time	min_left;
	int		own_count;

	now = cur_time();
	pthread_mutex_lock(&p->eating_mutex);
	{
		// cast long -> t_time before comparing/subtracting
		t_time max_at = (t_time)p->max_time_to_eat;
		own_left = (max_at > now) ? (max_at - now) : 0;
		own_count = p->eat_count;
	}
	pthread_mutex_unlock(&p->eating_mutex);
	if (own_left == 0 || own_left <= (t_time)URGENCY_MARGIN_MS)
		return (TRUE);
	min_left = min_time_to_die_left(p, now);
	if (own_left <= min_left + (t_time)URGENCY_MARGIN_MS)
		return (TRUE);
	if (own_left <= (t_time)(p->c->args[T_EAT] / 2))
		return (TRUE);
	return (own_count <= min_eat_count(p));
}

int	lock(t_philo *p)
{
	while (TRUE)
	{
		if (done(p) == TRUE)
			return (0);
		if (!can_eat_now(p))
		{
			ft_precise_sleep(100000ULL); // 0.1 ms
			continue;
		}
		pthread_mutex_lock(&p->c->waiter);
		if (p->c->waiter_slots > 0)
		{
			p->c->waiter_slots--;
			pthread_mutex_unlock(&p->c->waiter);
			return (1);
		}
		pthread_mutex_unlock(&p->c->waiter);
		ft_precise_sleep(100000ULL); // 0.1 ms
	}
}

void unlock(t_philo *p)
{
	pthread_mutex_lock(&p->c->waiter);
	p->c->waiter_slots++;
	if (p->c->waiter_slots > p->c->waiter_capacity)
		p->c->waiter_slots = p->c->waiter_capacity;
	pthread_mutex_unlock(&p->c->waiter);
}

int	get_fork(t_philo *p)
{
	int			first;
	int			second;
	uint64_t	start_ns;

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
	// Bounded try for the second fork: avoid holding one fork too long
	start_ns = now_ns();
	while (pthread_mutex_trylock(&p->c->forks[second]) != 0)
	{
		if (done(p) == TRUE)
		{
			pthread_mutex_unlock(&p->c->forks[first]);
			unlock(p);
			return (0);
		}
		if (now_ns() - start_ns >= SECOND_FORK_WAIT_NS)
		{
			// Give up to avoid blocking neighbors; retry later
			pthread_mutex_unlock(&p->c->forks[first]);
			unlock(p);
			ft_precise_sleep(SECOND_FORK_STEP_NS);
			return (0);
		}
		ft_precise_sleep(SECOND_FORK_STEP_NS);
	}
	// Only print after both forks are actually acquired
	print_game(p, FORK_TAKEN);
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