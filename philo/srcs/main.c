#include "philo.h"

static int	init_forks(t_ctx *ctx)
{
	int i;

	ctx->forks = (t_mutex *)malloc(sizeof(t_mutex) * ctx->total);
	if (!ctx->forks)
		return (ST_ERR_MALLOC);
	for (i = 0; i < (int)ctx->total; i++)
		pthread_mutex_init(&ctx->forks[i], NULL);
	return (ST_OK);
}

static void	cleanup(t_ctx *ctx, t_philo *philos)
{
	int i;

	if (philos)
	{
		for (i = 0; i < (int)ctx->total; i++)
			pthread_mutex_destroy(&philos[i].eating_mutex);
		free(philos);
	}
	if (ctx->forks)
	{
		for (i = 0; i < (int)ctx->total; i++)
			pthread_mutex_destroy(&ctx->forks[i]);
		free(ctx->forks);
	}
	pthread_mutex_destroy(&ctx->write_mutex);
	pthread_mutex_destroy(&ctx->dead);
	pthread_mutex_destroy(&ctx->time_eat);
	pthread_mutex_destroy(&ctx->finish);
	pthread_mutex_destroy(&ctx->start_lock);
	pthread_cond_destroy(&ctx->start_cond);
	// destroy waiter primitives
	pthread_mutex_destroy(&ctx->seats_lock);
	pthread_cond_destroy(&ctx->seats_cond);
}

int main(int argc, char **argv)
{
	t_ctx	ctx;
	t_philo	*philos = NULL;
	int		i;

	if (argc < 5 || argc > 6)
		return (ft_exit(MSG_ARGS_KO));
	memset(&ctx, 0, sizeof(ctx));
	init_ctx(&ctx, argc, argv);
	if (ctx.total == 0 || ctx.total > MAX_PHILOS || ctx.eat < MIN_MS
		|| ctx.sleep < MIN_MS || ctx.die < MIN_MS)
		return (ft_exit(MSG_ARGS_KO));
	if (init_forks(&ctx) != ST_OK)
		return (ft_exit("malloc forks failed\n"));
	if (init_philosophers(&ctx, &philos) != ST_OK)
		return (ft_exit("malloc philos failed\n"));

	// Set start time and initialize last_meal BEFORE creating threads
	ctx.start_t = get_time();
	for (i = 0; i < (int)ctx.total; i++)
		philos[i].last_meal = ctx.start_t;

	// Create philosopher threads (they will all wait at the barrier)
	for (i = 0; i < (int)ctx.total; i++)
		pthread_create(&philos[i].thread_id, NULL, philo_routine, &philos[i]);

	// Give threads a moment to reach the barrier
	usleep(5000);

	// Signal all threads to start simultaneously
	pthread_mutex_lock(&ctx.start_lock);
	ctx.started = 1;
	pthread_cond_broadcast(&ctx.start_cond);
	pthread_mutex_unlock(&ctx.start_lock);

	// Start monitor with a grace period to allow initial eating attempts
	usleep(10000); // 10ms grace period
	pthread_create(&ctx.monitor_thread, NULL, death_checker, &ctx);

	// Join philosophers
	for (i = 0; i < (int)ctx.total; i++)
		pthread_join(philos[i].thread_id, NULL);

	// Join monitor
	pthread_join(ctx.monitor_thread, NULL);

	cleanup(&ctx, philos);
	return (0);
}