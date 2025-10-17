/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_bonus.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glacroix <PGCL>                            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/07 19:30:55 by glacroix          #+#    #+#             */
/*   Updated: 2024/09/11 17:51:12 by glacroix         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo_bonus.h"

static void	status_catch_parent(t_data *data, t_philo *philo, int *status)
{
	int	j;
	int	exit_flag;

	j = -1;
	exit_flag = 0;
	while (1)
	{	
		waitpid(-1, status, WNOHANG);
		if (WIFEXITED(*status))
		{
			if (WEXITSTATUS(*status) == EXIT_DEATH)
			{
				while (++j < data->n_philos)
					kill(philo[j].pidc, SIGTERM);
				break ;
			}
			if (WEXITSTATUS(*status) == EXIT_EAT)
			{
				exit_flag += 1;
				if (exit_flag == data->n_philos)
					break ;
			}
		}
	}
}

static int	init_philos(t_data *data, t_philo *philo, int *i)
{
	philo->id = *i + 1;
	philo->data = data;
	philo->pidc = fork();
	if (philo->pidc < 0)
	{
		ft_putstr_fd("Error when creating process\n", 2);
		exit(EXIT_FAILURE);
	}
	return (0);
}

static int	sem_close_unlink(t_data *data)
{
	if (sem_close(data->forks) == -1)
		return (ft_putstr_fd("Error when closing the semaphore\n", 2), 2);
	if (sem_unlink("/forks") == -1)
		return (ft_putstr_fd("Error when unlinking the semaphore\n", 2), 1);
	if (sem_unlink("/print") == -1)
		return (ft_putstr_fd("Error when unlinking the semaphore\n", 2), 1);
	if (sem_unlink("/start_time") == -1)
		return (ft_putstr_fd("Error when unlinking the semaphore\n", 2), 1);
	if (sem_unlink("/data") == -1)
		return (ft_putstr_fd("Error when unlinking the semaphore\n", 2), 1);
	if (sem_unlink("/death") == -1)
		return (ft_putstr_fd("Error when unlinking the semaphore\n", 2), 1);
	return (0);
}

int	init_processes(t_data *data)
{
	t_philo	*philo;
	int		status;
	int		i;

	status = 0;
	i = -1;
	philo = malloc(sizeof(*philo) * data->n_philos);
	memset(philo, 0, sizeof(*philo) * data->n_philos);
	if (!philo)
		return (ft_putstr_fd("Error when mallocing philo\n", 2), 1);
	data->start_time = current_time();
	while (++i < data->n_philos)
	{
		init_philos(data, philo + i, &i);
		if (philo[i].pidc == 0)
			routine(philo + i);
	}
	status_catch_parent(data, philo, &status);
	sem_close_unlink(data);
	free(philo);
	return (0);
}

/*
1 800 200 200				problem -- death is not printed
a philo should die
4 310 200 100				problem -- death is not printed
a philo should die
4 200 205 200				problem	seems like deadlock..-but weird
a philo should die
5 800 200 200 7				problem with printing after the program ends
no one should die, simulation should stop after 7 eats
4 410 200 200 10
no one should die, simulation should stop after 10 eats
-5 600 200 200
should error and not run (no crashing)
4 -5 200 200
should error and not run (no crashing)
4 600 -5 200
should error and not run (no crashing)
4 600 200 -5
should error and not run (no crashing)
4 600 200 200 -5
should error and not run (no crashing)
//--------------------------------------------
5 800 200 200
no one should die
5 600 150 150
no one should die
4 410 200 200
no one should die
100 800 200 200
no one should die
105 800 200 200				small problem with printing -- everything is not on one line
no one should die
200 800 200 200		nobody knows --	happens something problem with printing
no one should die
*/
