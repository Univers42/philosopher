/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher_1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvassall <mvassall@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/05 17:39:27 by mvassall          #+#    #+#             */
/*   Updated: 2025/07/11 16:11:09 by mvassall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

t_exit_status	check_early_death(t_philo *phi)
{
	uint64_t	now;

	now = get_time_us();
	if (now > phi->death_ts + TOLERANCE_US)
	{
		change_phi_status(phi, PHI_DIED);
		return (EX_TIMEOUT);
	}
	if (phi->cfg->n_eating_rounds > 0
		&& phi->n_eating_counter > phi->cfg->n_eating_rounds)
	{
		change_phi_status(phi, PHI_FINISH);
		return (EX_PHILOSOPHER_DEAD);
	}
	return (EX_OK);
}

static t_exit_status	eating(t_philo *phi)
{
	uint64_t		end_eating_ts;
	t_exit_status	ex_status;

	ex_status = take_forks(phi);
	if (ex_status != EX_OK)
		return (ex_status);
	change_phi_status(phi, PHI_EATING);
	phi->death_ts = get_time_us() + phi->cfg->time_to_die_us;
	end_eating_ts = get_time_us() + phi->cfg->time_to_eat_us;
	ex_status = pass_time(phi, end_eating_ts);
	drop_forks(phi);
	if (ex_status == EX_OK)
	{
		phi->n_eating_counter++;
		phi->status = PHI_SLEEPING;
	}
	else if (ex_status == EX_TIMEOUT)
		change_phi_status(phi, PHI_DIED);
	else
		phi->status = PHI_FINISH;
	return (ex_status);
}

static t_exit_status	sleeping(t_philo *phi)
{
	uint64_t		end_sleeping_ts;
	t_exit_status	ex_status;

	if (get_time_us() >= phi->death_ts)
		return (change_phi_status(phi, PHI_DIED), EX_TIMEOUT);
	end_sleeping_ts = get_time_us() + phi->cfg->time_to_sleep_us;
	change_phi_status(phi, PHI_SLEEPING);
	ex_status = pass_time(phi, end_sleeping_ts);
	if (ex_status == EX_OK)
		phi->status = PHI_THINKING;
	else if (ex_status == EX_TIMEOUT)
		change_phi_status(phi, PHI_DIED);
	else
		phi->status = PHI_FINISH;
	return (ex_status);
}

static t_exit_status	thinking(t_philo *phi)
{
	uint64_t		end_thinking_ts;
	t_exit_status	ex_status;

	if (get_time_us() >= phi->death_ts)
		return (change_phi_status(phi, PHI_DIED), EX_TIMEOUT);
	end_thinking_ts = get_time_us() + phi->cfg->time_to_think_us;
	change_phi_status(phi, PHI_THINKING);
	ex_status = pass_time(phi, end_thinking_ts);
	if (ex_status == EX_OK)
		phi->status = PHI_EATING;
	else if (ex_status == EX_TIMEOUT)
		change_phi_status(phi, PHI_DIED);
	else
		phi->status = PHI_FINISH;
	return (ex_status);
}

void	*philosopher_routine(void *arg)
{
	t_philo	*phi;

	phi = (t_philo *)arg;
	while (1)
	{
		if (phi->status == PHI_EATING)
			eating(phi);
		else if (phi->status == PHI_SLEEPING)
			sleeping(phi);
		else if (phi->status == PHI_THINKING)
			thinking(phi);
		else
			break ;
	}
	increment_dead_counter(phi->cfg);
	return (NULL);
}
