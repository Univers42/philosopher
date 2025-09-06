/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ctr_lock.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/06 18:59:05 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/06 19:01:48 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/philosopher.h"

#ifdef FORK_CHECK
void	report_fork_violation(t_sim *sim, int fork_idx, int current, int attempted)
{
	fprintf(stderr,
		"[FORK_CHECK] VIOLATION: fork %d already owned by %d (attempt by %d)\n",
		fork_idx, current, attempted);
	sim->stop = 1;
}

void	mark_fork_acquire(t_philo *p, int fork_idx)
{
	int owner;

	owner = p->sim->fork_owner[fork_idx];
	if (owner != -1 && owner != p->id)
		report_fork_violation(p->sim, fork_idx, owner, p->id);
	else
		p->sim->fork_owner[fork_idx] = p->id;
}

void	mark_fork_release(t_philo *p, int fork_idx)
{
	int owner;

	owner = p->sim->fork_owner[fork_idx];
	if (owner == p->id || owner == -1)
		p->sim->fork_owner[fork_idx] = -1;
	else
		report_fork_violation(p->sim, fork_idx, owner, p->id);
}
#endif

void	lock_forks(t_philo *p)
{
	int left = p->left_fork;
	int right = p->right_fork;
	int first;
	int second;

	// Parity-based order to reduce starvation (deadlock still prevented by waiter)
	if (p->id % 2 == 0)
	{
		first = right;
		second = left;
	}
	else
	{
		first = left;
		second = right;
	}
	DBG(p->sim, p->id, "forks", "lock order %d -> %d", first, second);
	// ...existing code...
	if (first == second)
	{
		pthread_mutex_lock(&p->sim->forks[first]);
#ifdef FORK_CHECK
		mark_fork_acquire(p, first);
#endif
		log_status(p->sim, p->id, "has taken a fork");
		return ;
	}
	pthread_mutex_lock(&p->sim->forks[first]);
#ifdef FORK_CHECK
	mark_fork_acquire(p, first);
#endif
	log_status(p->sim, p->id, "has taken a fork");
	pthread_mutex_lock(&p->sim->forks[second]);
#ifdef FORK_CHECK
	mark_fork_acquire(p, second);
#endif
	log_status(p->sim, p->id, "has taken a fork");
}

void	unlock_forks(t_philo *p)
{
	int left = p->left_fork;
	int right = p->right_fork;

	if (left == right)
	{
#ifdef FORK_CHECK
		mark_fork_release(p, left);
#endif
		pthread_mutex_unlock(&p->sim->forks[left]);
		return ;
	}
	DBG(p->sim, p->id, "forks", "unlock order %s first",
		(p->id % 2 == 0) ? "left" : "right");
	if (p->id % 2 == 0)
	{
#ifdef FORK_CHECK
		mark_fork_release(p, left);
		mark_fork_release(p, right);
#endif
		pthread_mutex_unlock(&p->sim->forks[left]);
		pthread_mutex_unlock(&p->sim->forks[right]);
	}
	else
	{
#ifdef FORK_CHECK
		mark_fork_release(p, right);
		mark_fork_release(p, left);
#endif
		pthread_mutex_unlock(&p->sim->forks[right]);
		pthread_mutex_unlock(&p->sim->forks[left]);
	}
}