/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 23:07:33 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/16 23:07:52 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

t_status parse_args(int argc, char **argv, t_data *data, int *flags)
{
	t_status status = {STATE_OK, SEV_OK};
	if (argc < 5 || argc > 6)
	{
		*flags |= ERR_FATAL_PARSE;
		return (t_status){STATE_PARSE_ERR, SEV_FATAL};
	}
	data->n_philo = ft_strto_uchar(argv[1], NULL, RADIX_DEC);
	data->args[TT_DIE] = ft_strto_u64(argv[2], NULL, RADIX_DEC);
	data->args[TT_EAT] = ft_strto_u64(argv[3], NULL, RADIX_DEC);
	data->args[TT_SLEEP] = ft_strto_u64(argv[4], NULL, RADIX_DEC);
	if (argc == 6)
		data->n_meal = ft_strto_int(argv[5], NULL, RADIX_DEC);
	else
		data->n_meal = -1;
	if ((data->n_philo < 1 || data->n_philo > MAX_PHILOS) || data->args[TT_DIE] < 1 || data->args[TT_EAT] < 1 || data->args[TT_SLEEP] < 1 || (data->n_meal < -1))
	{
		*flags |= ERR_FATAL_PARSE;
		return (t_status){STATE_PARSE_ERR, SEV_FATAL};
	}
	if (data->n_meal == -1)
		*flags |= WARN_MEAL_UNLIMITED;
	return status;
}