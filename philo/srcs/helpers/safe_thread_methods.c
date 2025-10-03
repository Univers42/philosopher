/**
To prevent racing conditions we need to appropriately lock/unlock with mutexes before and after
writing or reading shared variables.
This way we prevent race conditions and ensures thread_safety
*/
void	set_stop(t_ctx *ctx, int value)
{
	pthread_mutex_lock(&ctx->dead);
	ctx->stop = value;
	pthread_mutex_unlock(&ctx->dead);
}

int	get_stop(t_ctx *ctx)
{
	int	value;

	pthread_mutex_lock(&ctx->dead);
	value = ctx->stop;
	pthread_mutex_unlock(&ctx->dead);
	return (value);
}

void	increment_np_p_finish(t_ctx *ctx)
{
	pthread_mutex_lock(&ctx->finish);
	ctx->np_p_finish++;
	pthread_mutex_unlock(&ctx->finish);
}

t_qty	get_nb_p_finish(t_ctx *ctx)
{
	t_qty value;

	pthread_mutex_lock(&ctx->finish);
	value = ctx->nb_p_finish;
	pthread_mutex_unlock(&ctx->finish);
	return (value);
}
