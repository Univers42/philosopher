/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log_helpers2.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 11:49:44 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 13:46:32 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	print_standard(long timestamp, int id, const char *msg)
{
	printf("%ld %d %s\n", timestamp, id, msg);
}

void	log_debug(t_philo *p, int level, const char *msg)
{
	long	timestamp;

	if (!DEBUG_MODE)
		return ;
	if (!p || !msg)
		return ;
	pthread_mutex_lock(&p->ctx->print);
	pthread_mutex_lock(&p->ctx->alive_m);
	if (p->ctx->all_alive || level == LOG_DEATH)
	{
		timestamp = get_time() - p->ctx->start_time;
		printf("%s [%6ldms] Philo %3d: %s\n",
			get_log_prefix(level), timestamp, p->id, msg);
	}
	pthread_mutex_unlock(&p->ctx->alive_m);
	pthread_mutex_unlock(&p->ctx->print);
}

void	print_state(t_philo *p, const char *msg)
{
	long	timestamp;
	int		should_print;

	if (!p || !msg)
		return ;
	if (DEBUG_MODE)
		track_action(p, msg);
	pthread_mutex_lock(&p->ctx->alive_m);
	should_print = p->ctx->all_alive;
	pthread_mutex_unlock(&p->ctx->alive_m);
	if (should_print)
	{
		pthread_mutex_lock(&p->ctx->print);
		timestamp = get_time() - p->ctx->start_time;
		if (ENHANCED_OUTPUT && !DEBUG_MODE)
			print_enhanced(timestamp, p->id, msg);
		else if (DEBUG_MODE)
			printf("%s%s │ %7ldms │ Philo %3d │ %s\033[0m\n",
				get_color_code(msg), get_state_icon(msg),
				timestamp, p->id, msg);
		else
			print_standard(timestamp, p->id, msg);
		pthread_mutex_unlock(&p->ctx->print);
	}
}

void	print_state_died(t_philo *p)
{
	long	timestamp;

	pthread_mutex_lock(&p->ctx->print);
	timestamp = get_time() - p->ctx->start_time;
	if (DEBUG_MODE)
	{
		p->ctx->debug_stats.death_time = timestamp;
		p->ctx->debug_stats.death_detected_time = get_time();
		p->ctx->debug_stats.death_philo_id = p->id;
	}
	if (ENHANCED_OUTPUT && !DEBUG_MODE)
		print_enhanced(timestamp, p->id, "died");
	else if (DEBUG_MODE)
		printf("\033[1;31m💀 │ %7ldms │ Philo %3d │ died\033[0m\n",
			timestamp, p->id);
	else
		print_standard(timestamp, p->id, "died");
	pthread_mutex_unlock(&p->ctx->print);
}
