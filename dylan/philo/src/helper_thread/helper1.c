/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helper1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 19:31:33 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/06 00:16:20 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <pthread.h>
#include <string.h>

void	get_value_safe(pthread_mutex_t *mutex, void *target,
		const void *source, size_t n)
{
	pthread_mutex_lock(mutex);
	ft_memcpy(target, source, n);
	pthread_mutex_unlock(mutex);
}

void	set_value_safe(pthread_mutex_t *mutex, void *target,
		const void *source, size_t n)
{
	pthread_mutex_lock(mutex);
	ft_memcpy(target, source, n);
	pthread_mutex_unlock(mutex);
}
