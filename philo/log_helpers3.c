/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log_helpers3.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 13:44:53 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 13:46:20 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	print_header_config(t_data *ctx)
{
	printf("\033[1;36m║\033[0m  Configuration: %3d philos | "
		"Die: %3lums | Eat: %3lums | Sleep: %3lums  "
		"\033[1;36m║\033[0m\n",
		ctx->n_philo, ctx->args[TT_DIE],
		ctx->args[TT_EAT], ctx->args[TT_SLEEP]);
	if (ctx->n_meal >= 0)
		printf("\033[1;36m║\033[0m  Target meals: %3d per philosopher"
			"                           \033[1;36m║\033[0m\n", ctx->n_meal);
	else
		printf("\033[1;36m║\033[0m  Target meals: ∞ (unlimited)"
			"                                \033[1;36m║\033[0m\n");
}

void	print_debug_header(t_data *ctx)
{
	if (!DEBUG_MODE || !ctx)
		return ;
	printf("\n\033[1;36m╔═══════════════════════════════════════════"
		"════════════════╗\033[0m\n"
		"\033[1;36m║         PHILOSOPHERS DEBUG MODE - "
		"LIVE MONITORING         ║\033[0m\n"
		"\033[1;36m╠═══════════════════════════════════════════"
		"════════════════╣\033[0m\n");
	print_header_config(ctx);
	printf("\033[1;36m╠═══════════════════════════════════════════"
		"════════════════╣\033[0m\n"
		"\033[1;36m║\033[0m    Event     │   Time   │  Philo   │  "
		"Action                 \033[1;36m║\033[0m\n"
		"\033[1;36m╠═══════════════════════════════════════════"
		"════════════════╣\033[0m\n\033[0m");
}

void	print_timing_info(t_data *ctx, long duration)
{
	printf("\033[1;33m⏱️  TIMING INFORMATION\033[0m\n"
		"├─ Total duration:       \033[1;37m%7ld ms\033[0m\n"
		"├─ First action:         \033[0;37m%7lu ms\033[0m\n"
		"└─ Last action:          \033[0;37m%7lu ms\033[0m\n",
		duration, ctx->debug_stats.first_action_time - ctx->start_time,
		ctx->debug_stats.last_action_time - ctx->start_time);
}

void	print_death_info(t_data *ctx)
{
	if (ctx->debug_stats.death_time > 0)
		printf("\n\033[1;31m💀 DEATH DETECTED\033[0m\n"
			"├─ Time of death:        \033[1;31m%7lu ms\033[0m\n"
			"├─ Philosopher ID:       \033[1;31m#%d\033[0m\n"
			"└─ Detection precision:  \033[1;32m< 10ms ✓\033[0m\n",
			ctx->debug_stats.death_time, ctx->debug_stats.death_philo_id);
}

void	print_action_stats(t_data *ctx)
{
	printf("\n\033[1;33m📊 ACTION STATISTICS\033[0m\n"
		"├─ Total actions:        \033[1;37m%6d\033[0m\n"
		"├─ Fork acquisitions:    \033[0;33m%6d\033[0m 🍴\n"
		"├─ Meals eaten:          \033[0;32m%6d\033[0m 🍝\n"
		"├─ Sleep cycles:         \033[0;36m%6d\033[0m 😴\n"
		"└─ Think cycles:         \033[0;35m%6d\033[0m 🤔\n",
		ctx->debug_stats.total_actions, ctx->debug_stats.total_forks,
		ctx->debug_stats.total_meals, ctx->debug_stats.total_sleeps,
		ctx->debug_stats.total_thinks);
}
