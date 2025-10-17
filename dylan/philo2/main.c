/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 17:21:52 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/16 23:17:12 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int main(int argc, char **argv)
{
	t_data	data;
	int		flags = 0;
	int		state;

	state = parse_args(argc, argv, &data, &flags);
	if (flags & ERR_FATAL_MASK)
		return log(state, STDERR_FILENO), 1;
		
	if (flags & WARN_MASK)
		ft_putstr_fd("Warning: unlimited meals (-1)\n", STDERR_FILENO);
	
	wait_threads(&data);
	cleanup(&data);
	return (0);
}