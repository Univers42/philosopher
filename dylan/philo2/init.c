/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 21:53:27 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/17 15:59:11 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "philo.h"

static int init_philos(t_data *c)
{
    int i;

    c->start_time = get_time();
    i = -1;
    while (++i < c->n_philo)
    {
        c->phi[i].id = (unsigned char)(i + 1);
        c->phi[i].ctx = c;
        c->phi[i].left_fork = &c->forks[i];
        c->phi[i].right_fork = &c->forks[(i + 1) % c->n_philo];
        c->phi[i].meals_eaten = 0;
        c->phi[i].last_meal = c->start_time;
        if (pthread_mutex_init(&c->phi[i].meal_lock, NULL) != 0)
            return (1);
    }
    return (0);
}

static int  init_forks(t_data *c)
{
    int i;

    i = -1;
    while (++i < c->n_philo)
    {
        if (pthread_mutex_init(&c->forks[i], NULL) != 0)
            return (1);
    }
    return (0);
}

static int init_shared_mutexes(t_data *data)
{
    if (pthread_mutex_init(&data->print, NULL) != 0)
        return (1);
    if (pthread_mutex_init(&data->alive_m, NULL) != 0)
    {
        pthread_mutex_destroy(&data->print);
        return (1);
    }
    if (pthread_mutex_init(&data->finished_m, NULL) != 0)
    {
        pthread_mutex_destroy(&data->print);
        pthread_mutex_destroy(&data->alive_m);
        return (1);
    }
    return (0);
}

int	init_simulation(t_data *rules)
{
    rules->forks = ft_calloc(rules->n_philo, sizeof(pthread_mutex_t));
    if (!rules->forks)
        return (ft_putstr_fd("Error allocating forks", 2), 1);
    rules->phi = ft_calloc(rules->n_philo, sizeof(t_philo));
    if (!rules->phi)
    {
        free(rules->forks);
        return (ft_putstr_fd("Error allocating philosophers", 2), 1);
    }
    if (init_forks(rules))
        return (ft_putstr_fd("Error initializing forks", 2), 1);
    if (init_shared_mutexes(rules))
        return (ft_putstr_fd("Error initializing shared mutexes", 2), 1);
    rules->all_alive = 1;
    rules->finished = 0;
    if (init_philos(rules))
        return (ft_putstr_fd("Error initializing philosophers", 2), 1);
    return (0);
}
