#include "philo.h"

void    cleanup(t_data *c)
{
	int	i;

	i = -1;
    while (++i < c->n_philo)
        pthread_join(c->phi[i].thread, NULL);
	//can't we destroy them together in one pass ?
	while (++i < c->n_philo)
		pthread_mutex_destroy(&c->phi[i].meal_lock);
	i = -1;
	while (++i < c->n_philo)
		pthread_mutex_destroy(&c->forks[i]);
	pthread_mutex_destroy(&c->print);
	pthread_mutex_destroy(&c->alive);
	pthread_mutex_destroy(&c->finished);
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
