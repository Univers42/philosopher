/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glacroix <glacroix@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/14 18:41:20 by glacroix          #+#    #+#             */
/*   Updated: 2023/09/22 18:28:53 by glacroix         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/philo.h"

/* void	ft_leaks(void)
{
	system("leaks -q philo");
} */

int	main(int argc, char **argv)
{
	t_data	data;

	if (args_wrong(argc, argv) == TRUE)
		return (ft_putstr_fd("Params are wrong\n", 2), 1);
	if (init_args(argc, argv, &data) == TRUE)
		return (ft_putstr_fd("Args are wrong\n", 2), 2);
	init_threads(&data);
	return (0);
}
