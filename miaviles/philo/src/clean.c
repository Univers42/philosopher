/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   clean.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 17:27:17 by miaviles          #+#    #+#             */
/*   Updated: 2025/10/17 14:47:03 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/philo.h"

//DONE
void	cleanup(t_rules *rules)
{
	int	i;

	i = 0;
	while (i < rules->nb_philosophers)
	{
		pthread_mutex_destroy(&rules->philos[i].meal_lock);
		i++;
	}
	i = 0;
	while (i < rules->nb_philosophers)
	{
		pthread_mutex_destroy(&rules->forks[i]);
		i++;
	}
	pthread_mutex_destroy(&rules->print_lock);
	pthread_mutex_destroy(&rules->alive_lock);
	pthread_mutex_destroy(&rules->finished_lock);
	free(rules->philos);
	free(rules->forks);
}
