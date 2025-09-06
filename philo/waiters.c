/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   waiters.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/06 19:00:18 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/06 19:00:48 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils/philosopher.h"

void	waiter_take_slot(t_philo *ph)
{
	t_sim *sim = ph->sim;
	int    id = ph->id;

	while (!sim_get_stop(sim))
	{
		pthread_mutex_lock(&sim->slot);
		if (sim_get_stop(sim))
		{
			pthread_mutex_unlock(&sim->slot);
			break ;
		}
		// Only block by urgent if that urgent philosopher is actually pending
		if (sim->urgent_id != -1 && sim->urgent_id != id
			&& sim->pending[sim->urgent_id - 1])
			DBG(sim, id, "waiter", "blocked by urgent=%d (slots=%d,next=%d)",
				sim->urgent_id, sim->slots, sim->next_id);
		if (sim->slots > 0 && (sim->urgent_id == -1 || sim->urgent_id == id
				|| !sim->pending[sim->urgent_id - 1]))
		{
			int best = best_pending_by_priority(sim);
			int allow = 0;
			if (sim->urgent_id == id || (best != -1 && best == id)
				|| sim->next_id == id || sim->pending[sim->next_id - 1] == 0)
				allow = 1;
			if (allow)
			{
				sim->slots--;
				if (sim->urgent_id == id)
					sim->urgent_id = -1;
				int k, found = 0;
				for (k = 0; k < sim->num_philos; k++)
				{
					int cand = ((id + k) % sim->num_philos) + 1;
					if (sim->pending[cand - 1])
					{
						sim->next_id = cand;
						found = 1;
						break ;
					}
				}
				if (!found)
					sim->next_id = (id % sim->num_philos) + 1;
				sim->pending[id - 1] = 0;
				DBG(sim, id, "waiter", "GRANTED slot (slots now %d, next=%d)",
					sim->slots, sim->next_id);
				pthread_mutex_unlock(&sim->slot);
				break ;
			}
			else
				DBG(sim, id, "waiter", "DENY (slots=%d,next=%d,pending[next]=%d)",
				 sim->slots, sim->next_id, sim->pending[sim->next_id - 1]);
		}
		pthread_mutex_unlock(&sim->slot);
		usleep(1000);
	}
}

void	waiter_give_slot(t_sim *sim)
{
	pthread_mutex_lock(&sim->slot);
	sim->slots++;
	pthread_mutex_unlock(&sim->slot);
}