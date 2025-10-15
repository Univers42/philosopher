#include "philo.h"
#include <sys/time.h>

typedef struct s_meta
{
	long	now;
	long	last_meal;
	long	limit;
	long	should_die_at;
	long	delay;
	long	last_rel;
	long	should_rel;
	long	now_rel;
}	t_meta;

int had_all_meals(t_data *c, int i)
{
    int meals_count;

	meals_count = 0;
    get_value_safe(&c->philos[i].eating_mutex, &meals_count, &c->philos[i].eat_count, sizeof(int));
    if (meals_count < c->args[MEALS_C])
        return (FALSE);
    return (TRUE);
}

int done(t_philo *p)
{
    int done;

	done = 0;
    get_value_safe(&p->c->m_finish, &done, &p->c->finish, sizeof(int));
    if (done)
        return (TRUE);
    return (FALSE);
}

int is_dead(t_philo *p)
{
    int				is_eating;
	t_time				expire_at;
	struct timeval		tv;
	unsigned long long	now_us;
	unsigned long long	expire_us;
	unsigned long long	start_us;

	pthread_mutex_lock(&(p->eating_mutex));
	is_eating = p->is_eating;
	expire_at = p->max_time_to_eat;
	pthread_mutex_unlock(&(p->eating_mutex));
	gettimeofday(&tv, NULL);
	now_us = (unsigned long long)tv.tv_sec * 1000000ULL + (unsigned long long)tv.tv_usec;
	start_us = (unsigned long long)p->c->program_start_time * 1000ULL;
	if (now_us < start_us)
		return (FALSE);
	expire_us = (unsigned long long)expire_at * 1000ULL;
	expire_us += (unsigned long long)DEATH_EPS_MS * 1000ULL;
	if (is_eating == FALSE && now_us > expire_us)
		return (TRUE);
	return (FALSE);
}

void	*set_death_and_exit(t_data *c, int i, int print_dead)
{
	t_meta			ctx;
	struct timeval	 tv;
	unsigned long long now_us;
	unsigned long long start_us;
	unsigned long long should_us;
	long long		 rel_us;
	long long		 delay_us;
	unsigned long long rel_ms;
	unsigned long long frac_us;
	
	if (print_dead)
	{
		pthread_mutex_lock(&c->philos[i].eating_mutex);
		ctx.last_meal = c->philos[i].last_meal;
		pthread_mutex_unlock(&c->philos[i].eating_mutex);
		ctx.limit = c->args[T_DIE];
		ctx.should_die_at = ft_positive_offset(ctx.last_meal, ctx.limit);
		gettimeofday(&tv, NULL);
		now_us = (unsigned long long)tv.tv_sec * 1000000ULL + (unsigned long long)tv.tv_usec;
		start_us = (unsigned long long)c->program_start_time * 1000ULL;
		should_us = (unsigned long long)ctx.should_die_at * 1000ULL;
		rel_us = (long long)now_us - (long long)start_us;
		if (rel_us < 0)
			rel_us = 0;
		delay_us = (long long)now_us - (long long)should_us;
		rel_ms = (unsigned long long)(rel_us / 1000LL);
		frac_us = (unsigned long long)(rel_us % 1000LL);
		ctx.now = (long)rel_ms;
		ctx.delay = (long)(delay_us / 1000LL);
		pthread_mutex_lock(&c->printing);
		pthread_mutex_lock(&c->m_finish);
		c->finish = TRUE;
		pthread_mutex_unlock(&c->m_finish);
		ctx.last_rel = ctx.last_meal - c->program_start_time;
		ctx.should_rel = ctx.should_die_at - c->program_start_time;
		ctx.now_rel = ctx.now;
		if (c->debug)
		{
			printf("\033[1;35m[DEBUG_DEATH] Philosopher %d\033[0m\n", c->philos[i].id);
			printf("\033[1;35m\t- last_meal_at: %ld ms\033[0m\n", ctx.last_rel);
			printf("\033[1;35m\t- should_die_at: %ld ms %ld\033[0m\n", ctx.should_rel, ctx.limit);
			printf("\033[1;35m\t- detected at: %ld ms\033[0m\n", ctx.now_rel);
			if (ctx.delay > MAX_DEATH_DELAY)
				printf("\033[1;35m\t-Warning: death detection delayed by more than %d ms!\033[0m\n", MAX_DEATH_DELAY);
		}
		// keep epsilon consideration for threshold calculation
		should_us += (unsigned long long)DEATH_EPS_MS * 1000ULL;
		printf("[%7llu.%03llu ms] [P:%3d] %s\n", rel_ms, frac_us, c->philos[i].id, DIED);
		pthread_mutex_unlock(&c->printing);
	}
	else
	{
		pthread_mutex_lock(&c->m_finish);
		c->finish = TRUE;
		pthread_mutex_unlock(&c->m_finish);
	}
	return (NULL);
}

void	*timeout_checker(void *p)
{
	t_data	*c;

	c = (t_data *)p;
	// wait until the program start time to avoid unsigned underflow in time_dif
	while (cur_time() < (t_time)c->program_start_time)
		usleep(50);
	while (TRUE)
	{
		if (done(&c->philos[0]) == TRUE)
			return (NULL);
		if (time_dif(c->program_start_time) >= c->timeout_ms)
		{
			pthread_mutex_lock(&c->m_finish);
			if (!c->finish)
				c->finish = TRUE;
			pthread_mutex_unlock(&c->m_finish);
			if (c->debug)
				debug_print(c, 0, "Timeout reached (%llu ms), stopping",
					(unsigned long long)c->timeout_ms);
			return (NULL);
		}
		usleep(1000);
	}
	return (NULL);
}
