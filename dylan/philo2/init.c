/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 21:53:27 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/16 23:35:06 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "philo.h"

int init_philos(t_data *c)
{
    unsigned char i;
    int res;

    i = -1;
    c->start_time = cur_time();
    while (++i < c->n_philo)
    {
        c->phi[i].id = i + 1;
        c->phi[i].l_fork = &c->forks[i];
        c->phi[i].right_fork = &c->forks[(i + 1) % c->n_philo];
        c->phi[i].meals_eaten = 0;
        c->phi[i].r_fork = (i + 1) % c->n_philo;
        c->phi[i].last_meal = cur_time();
        //c->phi[i].last_meal = c->start_time;
        c->phi[i].max_time_to_eat = ft_positive_offset(c->start_time, (t_time)(c->args[TT_DIE] + c->args[TT_EAT]));
        c->phi[i].is_eating = FALSE;
        c->phi[i].last_meal = 
        c->phi[i].left_fork = &c->forks[i];
    }
}

static int  init_forks(t_data *c)
{
    int i;
    int res;

    i = -1;
    while (++i < c->n_philo)
    {
        res = pthread_mutex_init(&c->forks[i], NULL);
        if (res != 0)
            return (log(NULL, STATE_ERR_FORK));
    }
    return (STATE_OK);
}

static int init_shared_mutexes(t_data *data)
{
    if (pthread_mutex_init(&data->print, NULL) != 0)
        return log(NULL, STATE_ERR_BASE);
    if (pthread_mutex_init(&data->alive, NULL) != 0)
    {
        pthread_mutex_destroy(&data->print);
        return log(NULL, STATE_ERR_BASE);
    }
    if (pthread_mutex_init(&data->finished, NULL) != 0)
    {
        pthread_mutex_destroy(&data->print);
        pthread_mutex_destroy(&data->alive);
        return log(NULL, STATE_ERR_BASE);
    }
    return (STATE_OK);
}

int	init_simulation(t_data *rules)
{
	rules->forks = malloc(sizeof(pthread_mutex_t) * rules->n_philo);
	if (!rules->forks)
		return (ft_error("Error allocating forks", 1));
	rules->phi = malloc(sizeof(t_philo) * rules->n_philo);
	if (!rules->phi)
	{
		free(rules->forks);
		return (ft_error("Error allocating philosophers", 1));
	}
	if (init_forks(rules))
		return (1);
	if (init_global_mutexes(rules))
		return (1);
	rules->all_alive = 1;
	rules->finished = 0;
	if (init_philos(rules))
		return (1);
	return (STATE_OK);
}
