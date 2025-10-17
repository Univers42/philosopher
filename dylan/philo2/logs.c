/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 23:07:10 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/16 23:25:44 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

t_state log(t_philo *p, t_state state)
{
	const char **msg = init_dispatcher();
	
	if (state == STATE_OK)
	{
		// ft_putnbr_fd(elapsed_time(p->ctx->start_time), STDOUT_FILENO);
		// ft_putstr_fd(" ms ", STDOUT_FILENO);
		// ft_putstr_fd(msg[state], STDOUT_FILENO);
		printf("%ld ms %d %s\n", elapsed_time(p->ctx->start_time),
				p->id, msg[state]);
	}
	if (state == STATE_WARN_BASE)
		printf("stub\n");
	if (state == STATE_INFO_BASE)
		printf("stub\n");
	if (state == STATE_ERR_BASE)
		ft_putstr_fd(msg[state], STDERR_FILENO);
	return (state);
}

void    print_state(t_philo *p, t_state state)
{
    long timestamp;

    pthread_mutex_lock(&(p->ctx->print));
    pthread_mutex_lock(&(p->ctx->alive));   //maybe alive is not really useful here..
    if (p->ctx->alive)
        log(p, state, STATE_OK);
    pthread_mutex_unlock(&(p->ctx->alive));
    pthread_mutex_unlock(&(p->ctx->print));
}

// we can improve this one by wrapping the print into one helper function
void    print_state_died(t_philo *p)
{
    long    timestamp;

    pthread_mutex_lock(&(p->ctx->print));
    printf("%lu %d died", elapsed_time(p->ctx->start_time), p->id);
    pthread_mutex_unlock(&(p->ctx->print));
}