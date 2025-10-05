#include "philo.h"

void	init_ctx(t_ctx *ctx, int argc, char **argv)
{
	ctx->total = (uint8_t)atoi(argv[1]);
	ctx->die = (t_time)strtoull(argv[2], NULL, 10);
	ctx->eat = (t_time)strtoull(argv[3], NULL, 10);
	ctx->sleep = (t_time)strtoull(argv[4], NULL, 10);
	if (argc == 6)
		ctx->m_eat = (t_qty)atoi(argv[5]);
	else
		ctx->m_eat = 0;
	ctx->start_t = 0;
	ctx->forks = NULL;
	pthread_mutex_init(&ctx->write_mutex, NULL);
	pthread_mutex_init(&ctx->dead, NULL);
	pthread_mutex_init(&ctx->time_eat, NULL);
	pthread_mutex_init(&ctx->finish, NULL);
	ctx->nb_p_finish = 0;
	ctx->stop = 0;
	ctx->monitor_thread = 0;
	pthread_mutex_init(&ctx->start_lock, NULL);
	pthread_cond_init(&ctx->start_cond, NULL);
	ctx->started = 0;
	ctx->debug = 0;
	// Enable debug if PHILO_DEBUG is set (any value)
	if (getenv("PHILO_DEBUG") && getenv("PHILO_DEBUG")[0] != '\0')
		ctx->debug = 1;
	ctx->philos = NULL; // set by init_philosophers/main

	// Waiter init: allow at most floor(total/2) concurrent eaters (>=1)
	pthread_mutex_init(&ctx->seats_lock, NULL);
	pthread_cond_init(&ctx->seats_cond, NULL);
	ctx->seats_avail = (ctx->total > 1) ? (ctx->total / 2) : 1;
}

int	init_philosophers(t_ctx *ctx, t_philo **philos)
{
	int	i;

	*philos = malloc(sizeof(t_philo) * ctx->total);
	if (!*philos)
		return (ST_ERR_MALLOC);
	i = 0;
	while (i < (int)ctx->total)
	{
		(*philos)[i].id = (uint8_t)(i + 1);
		(*philos)[i].lf = &ctx->forks[i];
		(*philos)[i].rf = &ctx->forks[(i + 1) % ctx->total];
		(*philos)[i].last_meal = 0; // set at synchronized start in thread
		(*philos)[i].eat_count = 0;
		(*philos)[i].ctx = ctx;
		pthread_mutex_init(&(*philos)[i].eating_mutex, NULL);
		(*philos)[i].thread_id = 0;
		i++;
	}
	ctx->philos = *philos;
	return (ST_OK);
}