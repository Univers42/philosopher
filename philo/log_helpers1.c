/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log_helpers1.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 11:45:37 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 13:28:38 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	track_action(t_philo *p, const char *msg)
{
	unsigned long	timestamp;

	if (!DEBUG_MODE)
		return ;
	timestamp = get_time();
	if (p->ctx->debug_stats.first_action_time == 0)
		p->ctx->debug_stats.first_action_time = timestamp;
	p->ctx->debug_stats.last_action_time = timestamp;
	p->ctx->debug_stats.total_actions++;
	if (msg[0] == 'h' && msg[4] == 't')
		p->ctx->debug_stats.total_forks++;
	else if (msg[0] == 'i' && msg[3] == 'e')
		p->ctx->debug_stats.total_meals++;
	else if (msg[0] == 'i' && msg[3] == 's' && msg[4] == 'l')
		p->ctx->debug_stats.total_sleeps++;
	else if (msg[0] == 'i' && msg[3] == 't')
		p->ctx->debug_stats.total_thinks++;
}

const char	*get_color_code(const char *msg)
{
	if (ft_strncmp(msg, "has taken a fork", 16) == 0)
		return ("\033[1;33m");
	if (ft_strncmp(msg, "is eating", 9) == 0)
		return ("\033[1;32m");
	if (ft_strncmp(msg, "is sleeping", 11) == 0)
		return ("\033[1;36m");
	if (ft_strncmp(msg, "is thinking", 11) == 0)
		return ("\033[1;35m");
	if (ft_strncmp(msg, "died", 4) == 0)
		return ("\033[1;31m");
	return ("\033[0m");
}

const char	*get_state_icon(const char *msg)
{
	if (ft_strncmp(msg, "has taken a fork", 16) == 0)
		return ("🍴");
	if (ft_strncmp(msg, "is eating", 9) == 0)
		return ("🍝");
	if (ft_strncmp(msg, "is sleeping", 11) == 0)
		return ("😴");
	if (ft_strncmp(msg, "is thinking", 11) == 0)
		return ("🤔");
	if (ft_strncmp(msg, "died", 4) == 0)
		return ("💀");
	return ("📋");
}

const char	*get_log_prefix(int level)
{
	if (level == LOG_INFO)
		return ("[ℹ️  INFO]");
	if (level == LOG_WARNING)
		return ("[⚠️  WARN]");
	if (level == LOG_ERROR)
		return ("[❌ ERROR]");
	if (level == LOG_DEATH)
		return ("[💀 DEATH]");
	return ("[🔍 DEBUG]");
}

void	print_enhanced(long timestamp, int id, const char *msg)
{
	const char	*color;
	const char	*icon;
	const char	*time_color;

	color = get_color_code(msg);
	icon = get_state_icon(msg);
	if (timestamp < 1000)
		time_color = "\033[1;32m";
	else if (timestamp < 5000)
		time_color = "\033[1;33m";
	else
		time_color = "\033[1;31m";
	printf("%s%s %s%6ldms\033[0m │ \033[1;37mPhilo %3d\033[0m │ %s%s\033[0m\n",
		color, icon, time_color, timestamp, id, color, msg);
}
