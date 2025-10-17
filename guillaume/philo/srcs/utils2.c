/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glacroix <glacroix@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/04 11:45:31 by glacroix          #+#    #+#             */
/*   Updated: 2023/09/04 16:31:10 by glacroix         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

size_t	ft_strlen(const char *str)
{
	size_t	x;

	x = 0;
	while (str[x] != '\0')
		x++;
	return (x);
}

void	ft_putstr_fd(char *s, int fd)
{
	if (!s)
		return ;
	if (s && fd >= 0)
		write(fd, s, ft_strlen(s));
}

void	free_memory(t_data *data, t_philo *philo)
{
	int i;

	free(philo);
	/* destroy all fork mutexes */
	i = -1;
	while (++i < data->nbr_philos)
		pthread_mutex_destroy(&data->forks[i]);
	free(data->forks);
	/* destroy global mutexes (ensure none are held at this point) */
	pthread_mutex_destroy(&data->ready_set);
	pthread_mutex_destroy(&data->log_mutex);
	pthread_mutex_destroy(&data->death_mutex);
	pthread_mutex_destroy(&data->finished_eating_mutex);
	pthread_mutex_destroy(&data->max_eat_mutex);
	pthread_mutex_destroy(&data->init_mutex);
}

int	thread_join(t_data *data, t_philo *philo)
{
	int	i;

	i = -1;
	while (++i < data->nbr_philos)
	{
		if (pthread_join(philo[i].thread, NULL) != 0)
			return (ft_putstr_fd("Error when joining thread\n", 2), 5);
	}
	return (0);
}
