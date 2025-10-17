/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 17:21:52 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/17 15:59:12 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <unistd.h>

int main(int argc, char **argv)
{
    t_data  data;
    int     flags = 0;
    t_status st;
    pthread_t th_monitor;
    pthread_t th_monitor_meals;

    st = parse_args(argc, argv, &data, &flags);
    if (flags & ERR_FATAL_MASK)
    {
        ft_putstr_fd(MSG_ERR_PARSE, STDERR_FILENO);
        return (1);
    }
    if (flags & WARN_MASK)
        ft_putstr_fd("Warning: unlimited meals (-1)", STDERR_FILENO);
    if (init_simulation(&data))
        return (1);
    if (start_threads(&data))
        return (cleanup(&data), 1);
    if (pthread_create(&th_monitor, NULL, monitor, &data) != 0)
        return (cleanup(&data), 1);
    if (pthread_create(&th_monitor_meals, NULL, monitor_meals, &data) != 0)
        return (cleanup(&data), 1);
    pthread_join(th_monitor, NULL);
    pthread_join(th_monitor_meals, NULL);
    wait_threads(&data);
    cleanup(&data);
    (void)st;
    return (0);
}