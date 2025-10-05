/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 20:48:11 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/06 00:19:26 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <pthread.h>
#include <stdarg.h>

int	print_game(t_philo *phils, char *str)
{
	int	should_print;

	pthread_mutex_lock(&phils->c->printing);
	get_value_safe(&phils->c->m_finish, &should_print,
		&phils->c->finish, sizeof(int));
	should_print = (should_print == FALSE);
	if (should_print || ft_strlen(str) == ft_strlen(DIED))
		printf(PRINTER, (long)time_dif(phils->c->program_start_time), phils->id, str);
	pthread_mutex_unlock(&phils->c->printing);
	return (FALSE);
}

// this is literally a copy pase of an old work
// it doens't comply the norminette
void	debug_print(t_data *c, int philo_id, const char *format, ...)
{
	va_list	args;

	if (!c->debug)
		return ;
	pthread_mutex_lock(&c->printing);
	printf("\033[1;35m[DEBUG] %ld ms - Philo %d: ",
		time_dif(c->program_start_time), philo_id);
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	printf("\033[0m\n");
	pthread_mutex_unlock(&c->printing);
}

void	print_one(t_time time)
{
	printf("\033[1;30m0 ms \033[1;31m 1 \033[1;33mhas taken a fork\n");
	printf("\033[1;30m%ld ms \033[1;31m 1 \033[1;31mdied\n", (long)ft_usleep(time));
}
