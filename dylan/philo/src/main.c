/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 20:51:02 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/05 23:43:53 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int main(int argc, char **argv)
{
    t_data  *c;
    int     i;

    c = malloc(sizeof(t_data));
    memset(c, 0, sizeof(t_data));
    if (init_args(c, argc, argv))
    {
        if (c->args[PHILO_C] == 1)
            print_one(c->args[T_DIE]);
        else
        {
            init_mutex(c);
            init_philosopher(c);
            init_checker(c);
            i = 0;
            while (i < c->args[PHILO_C])
            {
                pthread_join(c->philos[i].thread_id, NULL);
                i++;
            }
        }
        if (c->philos)
        {
            i = 0;
            while (i < c->args[PHILO_C])
            {
                pthread_mutex_destroy(&c->philos[i].eating_mutex);
                i++;
            }
        }
        if (c->forks)
        {
            i = 0;
            while (i < c->args[PHILO_C])
            {
                pthread_mutex_destroy(&c->forks[i]);
                i++;
            }
        }
        pthread_mutex_destroy(&c->printing);
        pthread_mutex_destroy(&c->m_finish);
        pthread_mutex_destroy(&c->death);
        pthread_mutex_destroy(&c->waiter);
        if (c->forks)
            free(c->forks);
        if (c->philos)
            free(c->philos);
    }
    else
        ft_putstr_fd(ARGS_KO, 2);
    free(c);
    return (0);
}