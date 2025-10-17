/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_utils_2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvassall <mvassall@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/05 13:43:51 by mvassall          #+#    #+#             */
/*   Updated: 2025/07/11 16:16:08 by mvassall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>

int	i_next(int i, int modulus)
{
	return ((i + 1) % modulus);
}

int	i_prev(int i, int modulus)
{
	return ((i - 1 + modulus) % modulus);
}

/**
  returns time stamp in micro seconds from simulation start
*/
uint64_t	get_ts_us(t_cfg_philo *cfg)
{
	return ((get_time_us() - cfg->start_ts));
}

int	philo_print(t_philo *philo, char *message)
{
	if (pthread_mutex_lock(&philo->cfg->print_mutex) != 0)
		return (0);
	printf("%ld %d %s\n", get_ts_us(philo->cfg) / 1000,
		philo->id + 1, message);
	pthread_mutex_unlock(&philo->cfg->print_mutex);
	return (1);
}

int	min(uint64_t a, uint64_t b)
{
	if (a < b)
		return (a);
	return (b);
}
