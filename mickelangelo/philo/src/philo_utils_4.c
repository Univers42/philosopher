/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils_4.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvassall <mvassall@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/06 15:56:04 by mvassall          #+#    #+#             */
/*   Updated: 2025/07/11 16:19:22 by mvassall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <pthread.h>
#include <stdlib.h>

int	change_phi_status(t_philo *phi, t_philo_status new_status)
{
	phi->status = new_status;
	if (new_status == PHI_EATING)
		return (philo_print(phi, "is eating"));
	if (new_status == PHI_THINKING)
		return (philo_print(phi, "is thinking"));
	if (new_status == PHI_SLEEPING)
		return (philo_print(phi, "is sleeping"));
	if (new_status == PHI_DIED)
		return (philo_print(phi, "died"));
	return (0);
}

static int	check_candidate(t_cfg_philo *cfg, int i)
{
	t_philo	*prev;
	t_philo	*next;

	if (cfg->philos[i].status != PHI_THINKING)
		return (0);
	prev = cfg->philos + i_prev(i, cfg->n_philosophers);
	next = cfg->philos + i_next(i, cfg->n_philosophers);
	return (prev->status != PHI_EATING && next->status != PHI_EATING);
}

int	get_next_to_eat(t_cfg_philo *cfg)
{
	int	i;
	int	i_found;

	i_found = -1;
	i = -1;
	while (++i < cfg->n_philosophers)
	{
		if (check_candidate(cfg, i))
			if (i_found < 0
				|| cfg->philos[i].death_ts < cfg->philos[i_found].death_ts)
				i_found = i;
	}
	return (i_found);
}

void	clean_up(t_cfg_philo *cfg)
{
	int	i;

	if (cfg == NULL)
		return ;
	pthread_mutex_destroy(&cfg->print_mutex);
	pthread_mutex_destroy(&cfg->m_dead_counter);
	i = -1;
	while (++i < cfg->n_philosophers)
		pthread_mutex_destroy(cfg->m_forks + i);
	free(cfg->m_forks);
	free(cfg->is_fork_in_use);
	free(cfg->philos);
	free(cfg);
}
