/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glacroix <glacroix@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/08 14:31:27 by glacroix          #+#    #+#             */
/*   Updated: 2023/09/26 15:31:23 by glacroix         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_bonus.h"

static void	logs(t_philo *philo, const char *str)
{
	sem_wait(philo->data->sem_print);
	printf("%lld %d %s\n", current_time()
		- philo->data->start_time, philo->id, str);
	sem_post(philo->data->sem_print);
}

void	takeforks(t_philo *philo)
{
	sem_wait(philo->data->forks);
	logs(philo, "has taken a fork");
	if (philo->data->n_philos == 1)
		ft_sleep(philo->data->time_to_die, philo);
	sem_wait(philo->data->forks);
	logs(philo, "has taken a fork");
}

void	eat(t_philo *philo)
{
	logs(philo, "is eating");
	ft_sleep(philo->data->time_to_eat, philo);
	philo->finished_eating_time = current_time() - philo->data->start_time;
	philo->ate_count++;
}

void	drop_n_sleep(t_philo *philo)
{
	sem_post(philo->data->forks);
	sem_post(philo->data->forks);
	if (philo->data->max_eating_cycles > -1
		&& philo->ate_count >= philo->data->max_eating_cycles)
		exit(EXIT_EAT);
	logs(philo, "is sleeping");
	ft_sleep(philo->data->time_to_sleep, philo);
	logs(philo, "is thinking");
}

void	routine(t_philo *philo)
{
	if (philo->id % 2 == 0)
		ft_sleep(50, philo);
	while (1)
	{
		if (philo->data->max_eating_cycles == 0)
			exit(EXIT_EAT);
		takeforks(philo);
		eat(philo);
		drop_n_sleep(philo);
	}
	return ;
}
