/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 23:07:33 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 04:13:17 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	validate_args(t_data *data)
{
	if (data->n_philo < 1 || data->n_philo > MAX_PHILOS)
		return (0);
	if (data->args[TT_DIE] < 1 || data->args[TT_EAT] < 1
		|| data->args[TT_SLEEP] < 1)
		return (0);
	if (data->n_meal < -1)
		return (0);
	return (1);
}

static void	assign_conv(t_data *data, char **argv, int argc)
{
	data->n_philo = ft_strto_uchar(argv[1], NULL, RADIX_DEC);
	data->args[TT_DIE] = ft_strto_ulong(argv[2], NULL, RADIX_DEC);
	data->args[TT_EAT] = ft_strto_ulong(argv[3], NULL, RADIX_DEC);
	data->args[TT_SLEEP] = ft_strto_ulong(argv[4], NULL, RADIX_DEC);
	if (argc == 6)
		data->n_meal = ft_strto_int(argv[5], NULL, RADIX_DEC);
	else
		data->n_meal = -1;
}

t_status	parse_args(int argc, char **argv, t_data *data, int *flags)
{
	t_status	status;

	status = (t_status){0, SEV_OK};
	if (!data || !flags)
		return ((t_status){1, SEV_FATAL});
	*flags = 0;
	if (argc < 5 || argc > 6)
	{
		*flags |= ERR_FATAL_PARSE;
		return ((t_status){1, SEV_FATAL});
	}
	assign_conv(data, argv, argc);
	if (!validate_args(data))
	{
		*flags |= ERR_FATAL_PARSE;
		return ((t_status){1, SEV_FATAL});
	}
	if (data->n_meal == -1)
		*flags |= WARN_MEAL_UNLIMITED;
	return (status);
}
