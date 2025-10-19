/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 15:14:33 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 13:49:54 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <unistd.h>

void	*monitor_meals(void *arg)
{
	t_data	*c;
	int		done;

	c = (t_data *)arg;
	if (c->n_meal == -1)
		return (NULL);
	while (get_simulation_state(c))
	{
		done = check_philo_ate(c);
		if (done == c->n_philo)
		{
			pthread_mutex_lock(&c->finished_m);
			if (!c->finished)
			{
				c->finished = 1;
				c->sim_state = SIM_STATE_COMPLETED;
				pthread_mutex_unlock(&c->finished_m);
				set_simulation_state(c, 0);
				break ;
			}
			pthread_mutex_unlock(&c->finished_m);
		}
		usleep(1000);
	}
	return (NULL);
}

void	*monitor(void *arg)
{
	t_data	*ctx;
	int		i;

	ctx = (t_data *)arg;
	while (get_simulation_state(ctx))
	{
		i = -1;
		while (++i < ctx->n_philo && get_simulation_state(ctx))
			if (check_philo_death(ctx, i))
				return (set_simulation_state(ctx, 0), NULL);
		usleep(1000);
	}
	return (NULL);
}

static void	*single_philo_routine(t_philo *philo)
{
	print_state(philo, "has taken a fork");
	ft_precise_usleep(philo->ctx->args[TT_DIE] + 1);
	return (NULL);
}

void	*routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->ctx->n_philo == 1)
		return (single_philo_routine(philo));
	if (philo->id % 2 == 0)
		ft_precise_usleep(philo->ctx->args[TT_EAT] / 2);
	while (get_simulation_state(philo->ctx))
	{
		if (!take_forks(philo))
			break ;
		if (!get_simulation_state(philo->ctx))
		{
			release_forks(philo);
			break ;
		}
		eat(philo);
		release_forks(philo);
		if (!get_simulation_state(philo->ctx))
			break ;
		sleep_and_think(philo);
	}
	return (NULL);
}

int	start_threads(t_data *ctx)
{
	int	i;
	int	res;

	i = -1;
	while (++i < ctx->n_philo)
	{
		res = pthread_create(&ctx->phi[i].thread, NULL, routine, &ctx->phi[i]);
		if (res != 0)
			return (ft_putstr_fd("Error creating threads", 2), 1);
	}
	return (0);
}
