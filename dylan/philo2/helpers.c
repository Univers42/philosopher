#include "philo.h"

void    cleanup(t_data *c)
{
    int i;

    if (!c)
        return;
    i = -1;
    while (++i < c->n_philo)
        pthread_mutex_destroy(&c->phi[i].meal_lock);
    i = -1;
    while (++i < c->n_philo)
        pthread_mutex_destroy(&c->forks[i]);
    pthread_mutex_destroy(&c->print);
    pthread_mutex_destroy(&c->alive_m);
    pthread_mutex_destroy(&c->finished_m);
    free(c->phi);
    free(c->forks);
}

void	*get_value_safe(pthread_mutex_t *mutex, void *target,
		const void *source, size_t n)
{
	pthread_mutex_lock(mutex);
	ft_memcpy(target, source, n);
	pthread_mutex_unlock(mutex);
    return (target);
}

void	*set_value_safe(pthread_mutex_t *mutex, void *target,
		const void *source, size_t n)
{
	pthread_mutex_lock(mutex);
	ft_memcpy(target, source, n);
	pthread_mutex_unlock(mutex);
    return (target);
}

int check_philo_death(t_data *c, int i)
{
    unsigned long   last_meal_time;
    unsigned long   current_time;

    pthread_mutex_lock(&c->phi[i].meal_lock);
    last_meal_time = c->phi[i].last_meal;
    pthread_mutex_unlock(&c->phi[i].meal_lock);
    current_time = get_time();
    if (current_time - last_meal_time > c->args[TT_DIE])
    {
        pthread_mutex_lock(&c->finished_m);
        if (!c->finished)
        {
            c->finished = 1;
            pthread_mutex_unlock(&c->finished_m);
            set_simulation_state(c, 0);
            print_state_died(&c->phi[i]);
            return (1);
        }
        pthread_mutex_unlock(&c->finished_m);
    }
    return (0);
}

//inneficient there is  alot of repetition with this calcul
int check_philo_ate(t_data *c)
{
    int i;
    int done;

    if (c->n_meal < 0)
        return (0);
    done = 0;
    i = -1;
    while (++i < c->n_philo)
    {
        pthread_mutex_lock(&c->phi[i].meal_lock);
        if (c->phi[i].meals_eaten >= c->n_meal)
            done++;
        pthread_mutex_unlock(&c->phi[i].meal_lock);
    }
    return (done);
}

