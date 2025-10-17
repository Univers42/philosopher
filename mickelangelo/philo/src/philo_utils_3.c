/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils_3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvassall <mvassall@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/05 16:39:42 by mvassall          #+#    #+#             */
/*   Updated: 2025/07/11 16:18:27 by mvassall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

static t_cfg_philo	*allocate_all(int n_philosophers)
{
	t_cfg_philo	*cfg;

	cfg = ft_calloc(1, sizeof(t_cfg_philo));
	if (cfg == NULL)
		return (NULL);
	cfg->philos = ft_calloc(n_philosophers, sizeof(t_philo));
	if (cfg->philos == NULL)
		return (free(cfg), NULL);
	cfg->m_forks = ft_calloc(n_philosophers, sizeof(pthread_mutex_t));
	if (cfg->m_forks == NULL)
		return (free(cfg->philos), free(cfg), NULL);
	cfg->is_fork_in_use = ft_calloc(n_philosophers, sizeof(int));
	if (cfg->is_fork_in_use == NULL)
		return (free(cfg->m_forks), free(cfg->philos), free(cfg), NULL);
	return (cfg);
}

t_cfg_philo	*init_cfg_philo(int *args)
{
	t_cfg_philo	*cfg;
	int			i;

	cfg = allocate_all(args[N_PHILOSOPHERS]);
	if (cfg == NULL)
		return (NULL);
	cfg->n_philosophers = args[N_PHILOSOPHERS];
	cfg->time_to_die_us = args[TIME_TO_DIE] * 1000;
	cfg->time_to_eat_us = args[TIME_TO_EAT] * 1000;
	cfg->time_to_sleep_us = args[TIME_TO_SLEEP] * 1000;
	cfg->time_to_think_us = THINKING_TIME_US;
	cfg->n_eating_rounds = args[N_EATING_ROUNDS];
	pthread_mutex_init(&cfg->print_mutex, NULL);
	pthread_mutex_init(&cfg->m_dead_counter, NULL);
	cfg->dead_counter = 0;
	cfg->start_ts = get_time_us();
	i = -1;
	while (++i < cfg->n_philosophers)
	{
		pthread_mutex_init(cfg->m_forks + i, NULL);
		cfg->is_fork_in_use[i] = 0;
	}
	return (cfg);
}

void	start_all_philos(t_cfg_philo *cfg)
{
	int	i;

	i = -1;
	while (++i < cfg->n_philosophers)
	{
		cfg->philos[i].id = i;
		cfg->philos[i].n_eating_counter = 0;
		cfg->philos[i].cfg = cfg;
		cfg->philos[i].death_ts = get_time_us() + cfg->time_to_die_us;
		cfg->philos[i].status = PHI_THINKING;
		pthread_create(&cfg->philos[i].thread, NULL,
			philosopher_routine, cfg->philos + i);
	}
}

int	has_someone_died(t_cfg_philo *cfg)
{
	int	n_dead;

	pthread_mutex_lock(&cfg->m_dead_counter);
	n_dead = cfg->dead_counter;
	pthread_mutex_unlock(&cfg->m_dead_counter);
	return (n_dead != 0);
}

void	increment_dead_counter(t_cfg_philo *cfg)
{
	pthread_mutex_lock(&cfg->m_dead_counter);
	cfg->dead_counter++;
	pthread_mutex_unlock(&cfg->m_dead_counter);
}
