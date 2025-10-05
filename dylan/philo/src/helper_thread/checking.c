#include "philo.h"

int had_all_meals(t_data *c, int i)
{
    int meals_count = 0;

    get_value_safe(&c->philos[i].eating_mutex, &meals_count, &c->philos[i].eat_count, sizeof(int));
    if (meals_count < c->args[MEALS_C])
        return (FALSE);
    return (TRUE);
}

int done(t_philo *p)
{
    int done = 0;

    get_value_safe(&p->c->m_finish, &done, &p->c->finish, sizeof(int));
    if (done)
        return (TRUE);
    return (FALSE);
}

int is_dead(t_philo *p)
{
    int     is_eating;
    t_time  last_meal;
    t_time  expire_at;
    t_time  now;

    pthread_mutex_lock(&(p->eating_mutex));
    is_eating = p->is_eating;
    last_meal = p->last_meal;
    pthread_mutex_unlock(&(p->eating_mutex));
    expire_at = ft_positive_offset(last_meal, p->c->args[T_DIE]);
    now = cur_time();
    if (is_eating == FALSE && now > expire_at)
        return (TRUE);
    return (FALSE);
}

void	*set_death_and_exit(t_data *c, int i, int print_dead)
{
	if (print_dead)
	{
		long	now, last_meal, limit, should_die_at, delay;
		long	now_rel, last_rel, should_rel;

		pthread_mutex_lock(&c->philos[i].eating_mutex);
		last_meal = c->philos[i].last_meal;
		pthread_mutex_unlock(&c->philos[i].eating_mutex);

		limit = c->args[T_DIE];
		should_die_at = ft_positive_offset(last_meal, limit);
		now = cur_time();
		delay = now - should_die_at;

		pthread_mutex_lock(&c->printing);
		pthread_mutex_lock(&c->m_finish);
		c->finish = TRUE;
		pthread_mutex_unlock(&c->m_finish);

		last_rel = last_meal - c->program_start_time;
		should_rel = should_die_at - c->program_start_time;
		now_rel = now - c->program_start_time;

		if (c->debug)
		{
			printf("\033[1;35m[DEBUG DEATH] Philosopher %d\033[0m\n", c->philos[i].id);
			printf("\033[1;35m  - last_meal_at: %ld ms\033[0m\n", last_rel);
			printf("\033[1;35m  - should_die_at: %ld ms (T_DIE=%ld)\033[0m\n", should_rel, limit);
			printf("\033[1;35m  - detected_at: %ld ms\033[0m\n", now_rel);
			printf("\033[1;35m  - delay: %ld ms\033[0m\n", delay);
			if (delay > MAX_DEATH_DELAY)
				printf("\033[1;31m  - WARNING: Death detection delayed by more than %d ms!\033[0m\n", MAX_DEATH_DELAY);
		}
		printf(PRINTER, now_rel, c->philos[i].id, DIED);
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
