/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 17:21:52 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 06:45:53 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <unistd.h>

static void	terminator(pthread_t th_monitor, pthread_t th_monitor_meals,
					t_data *data)
{
	pthread_join(th_monitor, NULL);
	pthread_join(th_monitor_meals, NULL);
	wait_threads(data);
	print_debug_summary(data);
	cleanup(data);
}

int	main(int argc, char **argv)
{
	t_data		data;
	int			flags;
	pthread_t	th_monitor;
	pthread_t	th_monitor_meals;

	flags = 0;
	parse_args(argc, argv, &data, &flags);
	if (flags & ERR_FATAL_MASK)
		return (ft_putstr_fd("Error: invalid arguments\n", STDERR_FILENO), 1);
	if (flags & WARN_MASK)
		ft_putstr_fd("Warning: unlimited meals (-1)", STDERR_FILENO);
	if (init_simulation(&data))
		return (1);
	print_debug_header(&data);
	if (start_threads(&data))
		return (cleanup(&data), 1);
	if (pthread_create(&th_monitor, NULL, monitor, &data) != 0)
		return (cleanup(&data), 1);
	if (pthread_create(&th_monitor_meals, NULL, monitor_meals, &data) != 0)
		return (cleanup(&data), 1);
	terminator(th_monitor, th_monitor_meals, &data);
	return (0);
}
