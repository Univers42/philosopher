/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mvassall <mvassall@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/06 20:28:58 by mvassall          #+#    #+#             */
/*   Updated: 2025/07/11 16:08:54 by mvassall         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdio.h>

int	main(int ac, char **av)
{
	int			input_values[5];
	t_cfg_philo	*cfg;
	int			j;

	if (!get_args(ac, av, input_values))
	{
		printf("ERROR: invalid input\n");
		printf("Usage: philo #philosopers time_to_die time_to_eat"
			" time_to_sleep [#rounds_eating]\n");
		return (1);
	}
	cfg = init_cfg_philo(input_values);
	if (cfg == NULL)
		return (printf("Unable to create operating environment\n"), 2);
	start_all_philos(cfg);
	j = -1;
	while (++j < cfg->n_philosophers)
		pthread_join(cfg->philos[j].thread, NULL);
	clean_up(cfg);
	return (0);
}
