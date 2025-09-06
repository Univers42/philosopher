/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   waiters.c                                          :+:      :+:    :+:   */
/*                                                                            */
/* ************************************************************************** */

#include "utils/philosopher.h"

static int	urgent_blocks(t_sim *sim, int id)
{
	return (sim->urgent_id != -1 && sim->urgent_id != id
		&& sim->pending[sim->urgent_id - 1]);
}

static int	slots_ok_and_urgent_ok(t_sim *sim, int id)
{
	return (sim->slots > 0 && (sim->urgent_id == -1 || sim->urgent_id == id
			|| !sim->pending[sim->urgent_id - 1]));
}

static void	ensure_next_valid_locked(t_sim *sim)
{
	int	i;
	int	cand;

	if (sim->next_id >= 1 && sim->next_id <= sim->num_philos
		&& sim->pending[sim->next_id - 1])
		return ;
	i = 0;
	while (i < sim->num_philos)
	{
		cand = ((sim->next_id + i) % sim->num_philos) + 1;
		if (sim->pending[cand - 1])
		{
			sim->next_id = cand;
			return ;
		}
		i++;
	}
	if (sim->next_id < 1)
		sim->next_id = 1;
}

static int	compute_allow(t_sim *sim, int id)
{
	if (sim->urgent_id == id)
		return (1);
	if (sim->next_id == id)
		return (1);
	return (0);
}

static void	pick_next_locked(t_sim *sim, int id)
{
	int	k;
	int	found;
	int	cand;

	found = 0;
	k = 0;
	while (k < sim->num_philos)
	{
		cand = ((id + k) % sim->num_philos) + 1;
		if (sim->pending[cand - 1])
		{
			sim->next_id = cand;
			found = 1;
			break ;
		}
		k++;
	}
	if (!found)
		sim->next_id = (id % sim->num_philos) + 1;
}

static void	grant_slot_and_unlock(t_sim *sim, int id)
{
	sim->slots--;
	if (sim->urgent_id == id)
		sim->urgent_id = -1;
	pick_next_locked(sim, id);
	sim->pending[id - 1] = 0;
	DBG(sim, id, "waiter", "GRANTED slot (slots now %d, next=%d)",
		sim->slots, sim->next_id);
	pthread_mutex_unlock(&sim->slot);
}

static int	can_grant_under_cap_locked(t_sim *sim)
{
	int	base;
	int	active;
	int	cap;
	int	in_use;

	base = (sim->num_philos > 1) ? (sim->num_philos - 1) : 1;
	active = sim->num_philos - sim->finished;
	if (active < 1)
		active = 1;
	cap = (active > 1) ? (active - 1) : 1;
	in_use = base - sim->slots;
	return (in_use < cap);
}

// return 0=deny/continue, 1=granted
static int	handle_available_locked(t_sim *sim, int id)
{
	if (!slots_ok_and_urgent_ok(sim, id))
		return (0);
	if (!can_grant_under_cap_locked(sim))
	{
		DBG(sim, id, "waiter", "cap reached (in_use=%d)",
			((sim->num_philos - 1) - sim->slots));
		return (0);
	}
	ensure_next_valid_locked(sim);
	if (compute_allow(sim, id))
	{
		grant_slot_and_unlock(sim, id);
		return (1);
	}
	DBG(sim, id, "waiter", "DENY (slots=%d,next=%d,pending[next]=%d)",
		sim->slots, sim->next_id, sim->pending[sim->next_id - 1]);
	return (0);
}

// return 0=continue, 1=stop, 2=granted
static int	waiter_try_iteration(t_sim *sim, int id)
{
	pthread_mutex_lock(&sim->slot);
	if (sim_get_stop(sim))
	{
		pthread_mutex_unlock(&sim->slot);
		return (1);
	}
	if (urgent_blocks(sim, id))
		DBG(sim, id, "waiter", "blocked by urgent=%d (slots=%d,next=%d)",
			sim->urgent_id, sim->slots, sim->next_id);
	if (handle_available_locked(sim, id))
		return (2);
	pthread_mutex_unlock(&sim->slot);
	return (0);
}

void	waiter_take_slot(t_philo *ph)
{
	t_sim	*sim;
	int		id;
	int		ret;

	sim = ph->sim;
	id = ph->id;
	while (!sim_get_stop(sim))
	{
		ret = waiter_try_iteration(sim, id);
		if (ret == 2)
			break ;
		if (ret == 1)
		{
			pthread_mutex_lock(&sim->slot);
			sim->pending[id - 1] = 0;
			pthread_mutex_unlock(&sim->slot);
			break ;
		}
		usleep(1000);
	}
}

void	waiter_give_slot(t_sim *sim)
{
	pthread_mutex_lock(&sim->slot);
	sim->slots++;
	if (sim->slots > (sim->num_philos > 1 ? sim->num_philos - 1 : 1))
		sim->slots = (sim->num_philos > 1 ? sim->num_philos - 1 : 1);
	pthread_mutex_unlock(&sim->slot);
}