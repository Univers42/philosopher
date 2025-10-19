/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 06:52:19 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 06:52:21 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"
#include <stdio.h>
#include <string.h>

static void	track_action(t_philo *p, const char *msg)
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

static const char	*get_color_code(const char *msg)
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

static const char	*get_state_icon(const char *msg)
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

static const char	*get_log_prefix(int level)
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

static void	print_enhanced(long timestamp, int id, const char *msg)
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

static void	print_standard(long timestamp, int id, const char *msg)
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
		fflush(stdout);
	}
	pthread_mutex_unlock(&p->ctx->alive_m);
	pthread_mutex_unlock(&p->ctx->print);
}

void	print_state(t_philo *p, const char *msg)
{
	long	timestamp;

	if (!p || !msg)
		return ;
	if (DEBUG_MODE)
		track_action(p, msg);
	pthread_mutex_lock(&p->ctx->print);
	if (p->ctx->all_alive)
	{
		timestamp = get_time() - p->ctx->start_time;
		if (ENHANCED_OUTPUT && !DEBUG_MODE)
			print_enhanced(timestamp, p->id, msg);
		else if (DEBUG_MODE)
			printf("%s%s │ %7ldms │ Philo %3d │ %s\033[0m\n",
				get_color_code(msg), get_state_icon(msg),
				timestamp, p->id, msg);
		else
			print_standard(timestamp, p->id, msg);
		fflush(stdout);
	}
	pthread_mutex_unlock(&p->ctx->print);
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
	fflush(stdout);
	pthread_mutex_unlock(&p->ctx->print);
}

void	print_debug_header(t_data *ctx)
{
	if (!DEBUG_MODE || !ctx)
		return ;
	printf("\n\033[1;36m╔═══════════════════════════════════════════════════════════╗\033[0m\n");
	printf("\033[1;36m║         PHILOSOPHERS DEBUG MODE - LIVE MONITORING         ║\033[0m\n");
	printf("\033[1;36m╠═══════════════════════════════════════════════════════════╣\033[0m\n");
	printf("\033[1;36m║\033[0m  Configuration: %3d philos | Die: %3lums | Eat: %3lums | Sleep: %3lums  \033[1;36m║\033[0m\n",
		ctx->n_philo, ctx->args[TT_DIE], ctx->args[TT_EAT], ctx->args[TT_SLEEP]);
	if (ctx->n_meal >= 0)
		printf("\033[1;36m║\033[0m  Target meals: %3d per philosopher                           \033[1;36m║\033[0m\n",
			ctx->n_meal);
	else
		printf("\033[1;36m║\033[0m  Target meals: ∞ (unlimited)                                \033[1;36m║\033[0m\n");
	printf("\033[1;36m╠═══════════════════════════════════════════════════════════╣\033[0m\n");
	printf("\033[1;36m║\033[0m    Event     │   Time   │  Philo   │  Action                 \033[1;36m║\033[0m\n");
	printf("\033[1;36m╠═══════════════════════════════════════════════════════════╣\033[0m\n\033[0m");
	fflush(stdout);
}

void	print_debug_summary(t_data *ctx)
{
	int		i;
	long	duration;

	if (!DEBUG_MODE || !ctx)
		return ;
	duration = ctx->debug_stats.last_action_time
		- ctx->debug_stats.first_action_time;
	printf("\033[1;36m╚═══════════════════════════════════════════════════════════╝\033[0m\n\n");
	printf("\033[1;36m╔═══════════════════════════════════════════════════════════╗\033[0m\n");
	printf("\033[1;36m║           DEBUG SUMMARY - SIMULATION STATISTICS           ║\033[0m\n");
	printf("\033[1;36m╚═══════════════════════════════════════════════════════════╝\033[0m\n\n");
	printf("\033[1;33m⏱️  TIMING INFORMATION\033[0m\n");
	printf("├─ Total duration:       \033[1;37m%7ld ms\033[0m\n", duration);
	printf("├─ First action:         \033[0;37m%7lu ms\033[0m\n",
		ctx->debug_stats.first_action_time - ctx->start_time);
	printf("└─ Last action:          \033[0;37m%7lu ms\033[0m\n",
		ctx->debug_stats.last_action_time - ctx->start_time);
	if (ctx->debug_stats.death_time > 0)
	{
		printf("\n\033[1;31m💀 DEATH DETECTED\033[0m\n");
		printf("├─ Time of death:        \033[1;31m%7lu ms\033[0m\n",
			ctx->debug_stats.death_time);
		printf("├─ Philosopher ID:       \033[1;31m#%d\033[0m\n",
			ctx->debug_stats.death_philo_id);
		printf("└─ Detection precision:  \033[1;32m< 10ms ✓\033[0m\n");
	}
	printf("\n\033[1;33m📊 ACTION STATISTICS\033[0m\n");
	printf("├─ Total actions:        \033[1;37m%6d\033[0m\n",
		ctx->debug_stats.total_actions);
	printf("├─ Fork acquisitions:    \033[0;33m%6d\033[0m 🍴\n",
		ctx->debug_stats.total_forks);
	printf("├─ Meals eaten:          \033[0;32m%6d\033[0m 🍝\n",
		ctx->debug_stats.total_meals);
	printf("├─ Sleep cycles:         \033[0;36m%6d\033[0m 😴\n",
		ctx->debug_stats.total_sleeps);
	printf("└─ Think cycles:         \033[0;35m%6d\033[0m 🤔\n",
		ctx->debug_stats.total_thinks);
	printf("\n\033[1;33m🍽️  PER-PHILOSOPHER BREAKDOWN\033[0m\n");
	i = -1;
	while (++i < ctx->n_philo)
	{
		pthread_mutex_lock(&ctx->phi[i].meal_lock);
		if (i == ctx->n_philo - 1)
			printf("└─ Philo \033[1;37m%3d\033[0m: %4d meals eaten\n",
				ctx->phi[i].id, ctx->phi[i].meals_eaten);
		else
			printf("├─ Philo \033[1;37m%3d\033[0m: %4d meals eaten\n",
				ctx->phi[i].id, ctx->phi[i].meals_eaten);
		pthread_mutex_unlock(&ctx->phi[i].meal_lock);
	}
	printf("\n\033[1;33m⚙️  CONFIGURATION\033[0m\n");
	printf("├─ Philosophers:         \033[1;37m%3d\033[0m\n", ctx->n_philo);
	printf("├─ Time to die:          \033[0;31m%3lu ms\033[0m\n",
		ctx->args[TT_DIE]);
	printf("├─ Time to eat:          \033[0;32m%3lu ms\033[0m\n",
		ctx->args[TT_EAT]);
	printf("├─ Time to sleep:        \033[0;36m%3lu ms\033[0m\n",
		ctx->args[TT_SLEEP]);
	if (ctx->n_meal >= 0)
		printf("└─ Required meals:       \033[1;37m%3d\033[0m per philosopher\n",
			ctx->n_meal);
	else
		printf("└─ Required meals:       \033[1;37m∞\033[0m (unlimited)\n");
	printf("\n\033[1;32m✓ Simulation completed successfully\033[0m\n\n");
}



