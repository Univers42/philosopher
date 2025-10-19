/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_state.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 02:32:27 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 04:11:09 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	get_simulation_state(t_data *ctx)
{
	int	state;

	pthread_mutex_lock(&ctx->alive_m);
	state = ctx->all_alive;
	pthread_mutex_unlock(&ctx->alive_m);
	return (state);
}

void	set_simulation_state(t_data *ctx, int state)
{
	pthread_mutex_lock(&ctx->alive_m);
	ctx->all_alive = state;
	pthread_mutex_unlock(&ctx->alive_m);
}

void	wait_threads(t_data *ctx)
{
	int	i;

	i = 0;
	while (i < ctx->n_philo)
	{
		pthread_join(ctx->phi[i].thread, NULL);
		i++;
	}
}
