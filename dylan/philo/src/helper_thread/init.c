/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 20:09:51 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/06 01:00:44 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	init_mutex(t_data *c)
{
	int	i;

	pthread_mutex_init(&c->printing, NULL);
	pthread_mutex_init(&c->death, NULL);
	pthread_mutex_init(&c->m_finish, NULL);
	pthread_mutex_init(&c->waiter, NULL);
	c->waiter_slots = c->args[PHILO_C] - 1;
	c->forks = ft_calloc(c->args[PHILO_C], sizeof(t_mutex));
	if (!c->forks)
		return (MALLOC_KO);
	i = -1;
	while (++i < c->args[PHILO_C])
		pthread_mutex_init(&c->forks[i], NULL);
	return (MUTEX_OK);
}

int	init_checker(t_data *c)
{
	pthread_t	meals_thread;
	pthread_t	death_thread;
	pthread_t	timeout_thread;

	// set a sane default runtime timeout
	c->timeout_ms = DEFAULT_TIMEOUT_MS;

	if (c->args[MEALS_C] > 0)
	{
		pthread_create(&meals_thread, NULL, &meals_checker, (void *)c);
	}
	pthread_create(&death_thread, NULL, &death_checker, (void *)c);
	pthread_create(&timeout_thread, NULL, &timeout_checker, (void *)c);

	// Join threads: all monitors exit when finish == TRUE
	if (c->args[MEALS_C] > 0)
		pthread_join(meals_thread, NULL);
	pthread_join(death_thread, NULL);
	pthread_join(timeout_thread, NULL);
	return (0);
}

int	init_philosopher(t_data *c)
{
	int	i;

	c->philos = ft_calloc(c->args[PHILO_C], sizeof(t_philo));
	if (!c->philos)
		return (MALLOC_KO);
	i = -1;
	while (++i < c->args[PHILO_C])
	{
		c->philos[i].id = i + 1;
		c->philos[i].l_fork = i;
		c->philos[i].r_fork = (i + 1) % c->args[PHILO_C];
		c->philos[i].last_meal = cur_time();
		c->philos[i].c = c;
		pthread_mutex_init(&(c->philos[i].eating_mutex), NULL);
		if (pthread_create(&c->philos[i].thread_id, NULL,
				&dinner, (void *)&c->philos[i]))
			return (0);
		usleep(100);
	}
	return (1);
}

int	init_args(t_data *c, int argc, char **argv)
{
	if (argc < 5 || argc > 6)
		return (0);
	c->args[PHILO_C] = ft_strto_uchar(argv[1], NULL, RADIX_DEC);
	c->args[T_DIE] = ft_strtou64(argv[2], NULL, RADIX_DEC);
	c->args[T_EAT] = ft_strtou64(argv[3], NULL, RADIX_DEC);
	c->args[T_SLEEP] = ft_strtou64(argv[4], NULL, RADIX_DEC);
	c->finish = FALSE;
	c->debug = DEBUG_MODE;
	// default timeout (can be adjusted later if desired)
	c->timeout_ms = DEFAULT_TIMEOUT_MS;
	if (argc == 6)
		c->args[MEALS_C] = ft_strto_int(argv[5], NULL, RADIX_DEC);
	else
		c->args[MEALS_C] = -1;
	if (c->args[PHILO_C] < 1 || c->args[PHILO_C] > 250
		|| c->args[T_DIE] < 1 || c->args[T_EAT] < 1 || c->args[T_SLEEP] < 1
		|| c->args[T_DIE] > MAX_TIME_MS
		|| c->args[T_EAT] > MAX_TIME_MS
		|| c->args[T_SLEEP] > MAX_TIME_MS
		|| (argc == 6 && c->args[MEALS_C] < 1))
		return (0);
	c->program_start_time = cur_time();
	if (c->program_start_time == 0)
		return (0);
	return (1);
}
