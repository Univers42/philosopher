#include "philo.h"
#include <stdarg.h>

// Debug logger guarded by write_mutex to avoid interleaving
static void	dbg(t_philo *ph, const char *tag, const char *fmt, ...)
{
	t_ctx *ctx = ph->ctx;
	va_list ap;

	if (!ctx->debug && !DEBUG)
		return;
	pthread_mutex_lock(&ctx->write_mutex);
	printf("[DBG] %s t=%" PRIu64 "ms ph=%d ", tag, get_time() - ctx->start_t, ph->id);
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	printf("\n");
	pthread_mutex_unlock(&ctx->write_mutex);
}

// Total order: lock lower-addressed mutex first with trylock on second
void	lock_forks(t_philo *ph)
{
	t_mutex *first = ph->lf;
	t_mutex *second = ph->rf;
	int      idx_l = (int)(ph->lf - ph->ctx->forks);
	int      idx_r = (int)(ph->rf - ph->ctx->forks);
	t_ctx   *ctx   = ph->ctx;
	t_time   t0;

	// Order by address to break cycles
	if (first > second)
	{
		first = ph->rf;
		second = ph->lf;
	}

	while (!get_stop(ctx))
	{
		t0 = get_time();
		pthread_mutex_lock(first);
		write_status(MSG_FORK_TAKEN, ph);
		dbg(ph, "FORK", "first_lock idx(l,r)=(%d,%d)", idx_l, idx_r);

		// Try to get the second fork; if unavailable, release and back off
		if (pthread_mutex_trylock(second) == 0)
		{
			write_status(MSG_FORK_TAKEN, ph);
			dbg(ph, "FORK", "both_locked wait_ms=%" PRIu64, (get_time() - t0));
			return;
		}
		pthread_mutex_unlock(first);
		// Backoff to avoid convoy; small sleep
		usleep(200);
	}
}

// Try once to get both forks with total order; return 1 on success, 0 otherwise
static int	lock_forks_once(t_philo *ph)
{
	t_mutex *first = ph->lf;
	t_mutex *second = ph->rf;
	int      idx_l = (int)(ph->lf - ph->ctx->forks);
	int      idx_r = (int)(ph->rf - ph->ctx->forks);
	//t_ctx   *ctx   = ph->ctx;
	t_time   t0;

	// Order by address to break cycles
	if (first > second)
	{
		first = ph->rf;
		second = ph->lf;
	}

	t0 = get_time();
	pthread_mutex_lock(first);
	write_status(MSG_FORK_TAKEN, ph);
	dbg(ph, "FORK", "first_lock idx(l,r)=(%d,%d)", idx_l, idx_r);

	// Try to get the second fork; if unavailable, release and report failure
	if (pthread_mutex_trylock(second) == 0)
	{
		write_status(MSG_FORK_TAKEN, ph);
		dbg(ph, "FORK", "both_locked wait_ms=%" PRIu64, (get_time() - t0));
		return (1);
	}
	pthread_mutex_unlock(first);
	return (0);
}

static void	unlock_forks(t_philo *ph)
{
	if (ph->lf == ph->rf)
	{
		pthread_mutex_unlock(ph->lf);
		return;
	}
	pthread_mutex_unlock(ph->lf);
	pthread_mutex_unlock(ph->rf);
}

void	write_status(const char *str, t_philo *ph)
{
	t_ctx  *ctx = ph->ctx;
	t_time	now = get_time();

	pthread_mutex_lock(&ctx->write_mutex);
	if (!get_stop(ctx) || strcmp(str, MSG_DIED) == 0)
		printf("%" PRIu64 " %d %s\n", (now - ctx->start_t), ph->id, str);
	pthread_mutex_unlock(&ctx->write_mutex);
}

void	*death_checker(void *arg)
{
	t_ctx	*ctx = (t_ctx *)arg;

	while (!get_stop(ctx))
	{
		int		i;
		int		all_done = (ctx->m_eat > 0);
		t_time	now = get_time();

		for (i = 0; i < (int)ctx->total; i++)
		{
			t_philo *ph = &ctx->philos[i];
			t_time elapsed;
			int    eaten;

			pthread_mutex_lock(&ph->eating_mutex);
			elapsed = now - ph->last_meal;
			eaten = ph->eat_count;
			pthread_mutex_unlock(&ph->eating_mutex);

			if (elapsed > ctx->die)
			{
				// Detailed diagnostics on death
				if (ctx->debug || DEBUG)
				{
					pthread_mutex_lock(&ctx->write_mutex);
					printf("[DBG] DEATH t=%" PRIu64 "ms ph=%d elapsed=%" PRIu64
					       " die=%" PRIu64 " last_meal=%" PRIu64 " eaten=%d\n",
					       now - ctx->start_t, ph->id, elapsed, ctx->die,
					       ph->last_meal - ctx->start_t, eaten);
					pthread_mutex_unlock(&ctx->write_mutex);
				}
				write_status(MSG_DIED, ph);
				set_stop(ctx, 1);
				return (NULL);
			}
			if (ctx->m_eat > 0 && eaten < (int)ctx->m_eat)
				all_done = 0;
		}
		if (all_done)
		{
			set_stop(ctx, 1);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}

void	*philo_routine(void *arg)
{
	t_philo	*ph = (t_philo *)arg;
	t_ctx	*ctx = ph->ctx;

	// Start barrier: wait for global start
	pthread_mutex_lock(&ctx->start_lock);
	while (!ctx->started)
		pthread_cond_wait(&ctx->start_cond, &ctx->start_lock);
	pthread_mutex_unlock(&ctx->start_lock);

	dbg(ph, "START", "thread started at t=%" PRIu64 "ms", get_time() - ctx->start_t);

	// Single philosopher: can never eat; take one fork then die naturally
	if (ctx->total == 1)
	{
		pthread_mutex_lock(ph->lf);
		write_status(MSG_FORK_TAKEN, ph);
		precise_sleep(ctx->die + 1);
		pthread_mutex_unlock(ph->lf);
		return (NULL);
	}

	while (!get_stop(ctx))
	{
		write_status(MSG_THINKING, ph);

		// Acquire a seat before attempting forks
		waiter_acquire(ctx);

		// Attempt to get both forks once under the seat
		if (!lock_forks_once(ph))
		{
			// Could not get both forks quickly; free the seat and back off
			waiter_release(ctx);
			usleep(200);
			continue;
		}

		// Eat
		pthread_mutex_lock(&ph->eating_mutex);
		ph->last_meal = get_time();
		pthread_mutex_unlock(&ph->eating_mutex);
		write_status(MSG_EATING, ph);
		precise_sleep(ctx->eat);
		pthread_mutex_lock(&ph->eating_mutex);
		ph->eat_count++;
		pthread_mutex_unlock(&ph->eating_mutex);

		// Release forks and the seat
		unlock_forks(ph);
		waiter_release(ctx);

		// Sleep
		write_status(MSG_SLEEPING, ph);
		precise_sleep(ctx->sleep);
	}
	return (NULL);
}
