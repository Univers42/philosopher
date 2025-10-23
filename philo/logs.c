/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 06:52:19 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 13:46:42 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	print_philo_meals(t_data *ctx)
{
	int	i;

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
}

static void	print_config_summary(t_data *ctx)
{
	printf("\n\033[1;33m⚙️  CONFIGURATION\033[0m\n"
		"├─ Philosophers:         \033[1;37m%3d\033[0m\n"
		"├─ Time to die:          \033[0;31m%3lu ms\033[0m\n"
		"├─ Time to eat:          \033[0;32m%3lu ms\033[0m\n"
		"├─ Time to sleep:        \033[0;36m%3lu ms\033[0m\n",
		ctx->n_philo, ctx->args[TT_DIE],
		ctx->args[TT_EAT], ctx->args[TT_SLEEP]);
	if (ctx->n_meal >= 0)
		printf("└─ Required meals:       \033[1;37m%3d\033[0m"
			" per philosopher\n", ctx->n_meal);
	else
		printf("└─ Required meals:       \033[1;37m∞\033[0m"
			" (unlimited)\n");
	printf("\n\033[1;32m✓ Simulation completed successfully\033[0m\n");
}

void	print_debug_summary(t_data *ctx)
{
	long	duration;

	if (!DEBUG_MODE || !ctx)
		return ;
	duration = ctx->debug_stats.last_action_time
		- ctx->debug_stats.first_action_time;
	printf("\033[1;36m╚═══════════════════════════════════════════"
		"════════════════╝\033[0m\n\n"
		"\033[1;36m╔═══════════════════════════════════════════"
		"════════════════╗\033[0m\n"
		"\033[1;36m║           DEBUG SUMMARY - SIMULATION "
		"STATISTICS           ║\033[0m\n"
		"\033[1;36m╚═══════════════════════════════════════════"
		"════════════════╝\033[0m\n\n");
	print_timing_info(ctx, duration);
	print_death_info(ctx);
	print_action_stats(ctx);
	print_philo_meals(ctx);
	print_config_summary(ctx);
}
