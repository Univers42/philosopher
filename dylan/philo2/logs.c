/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 23:07:10 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/17 15:59:11 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdio.h>
#include <string.h>

void    print_state(t_philo *p, const char *msg)
{
    long timestamp;

    if (!p || !msg)
        return;
    pthread_mutex_lock(&p->ctx->print);
    pthread_mutex_lock(&p->ctx->alive_m);
    if (p->ctx->all_alive)
    {
        timestamp = get_time() - p->ctx->start_time;
        printf("%ld %d %s\n", timestamp, p->id, msg);
        fflush(stdout);
    }
    pthread_mutex_unlock(&p->ctx->alive_m);
    pthread_mutex_unlock(&p->ctx->print);
}

void    print_state_died(t_philo *p)
{
    long timestamp;

    pthread_mutex_lock(&p->ctx->print);
    timestamp = get_time() - p->ctx->start_time;
    printf("%ld %d died\n", timestamp, p->id);
    fflush(stdout);
    pthread_mutex_unlock(&p->ctx->print);
}

