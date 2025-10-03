#include "philo.h"

#define LEFT(id, n) (((id) + (n) - 1) % (n))
#define RIGHT(id, n) (((id) + 1) % (n))

// Test if philosopher can eat (both neighbors are not eating)
static void	test(t_monitor *mon, int philo_id)
{
	int	left;
	int	right;

	left = LEFT(philo_id, mon->num_philos);
	right = RIGHT(philo_id, mon->num_philos);
	if (mon->state[philo_id] == HUNGRY
		&& mon->state[left] != EATING
		&& mon->state[right] != EATING)
	{
		mon->state[philo_id] = EATING;
		pthread_cond_signal(&mon->philo_cond[philo_id]);
	}
}

// Initialize monitor
t_monitor	*monitor_init(t_args args)
{
	t_monitor	*mon;
	int			i;

	mon = malloc(sizeof(t_monitor));
	if (!mon)
		return (NULL);
	mon->num_philos = args.nfork;
	mon->args = args;
	mon->simulation_end = false;
	mon->state = malloc(sizeof(t_philo_state) * mon->num_philos);
	mon->philo_cond = malloc(sizeof(pthread_cond_t) * mon->num_philos);
	if (!mon->state || !mon->philo_cond)
	{
		free(mon->state);
		free(mon->philo_cond);
		free(mon);
		return (NULL);
	}
	i = -1;
	while (++i < mon->num_philos)
	{
		mon->state[i] = THINKING;
		pthread_cond_init(&mon->philo_cond[i], NULL);
	}
	pthread_mutex_init(&mon->lock, NULL);
	pthread_mutex_init(&mon->print_lock, NULL);
	pthread_mutex_init(&mon->start_lock, NULL);
	pthread_cond_init(&mon->start_cond, NULL);
	mon->started = false;
	mon->print_stopped = false; // init
	return (mon);
}

// Destroy monitor
void	monitor_destroy(t_monitor *mon)
{
	int	i;

	if (!mon)
		return ;
	i = -1;
	while (++i < mon->num_philos)
		pthread_cond_destroy(&mon->philo_cond[i]);
	pthread_mutex_destroy(&mon->lock);

	// destroy new primitives
	pthread_mutex_destroy(&mon->print_lock);
	pthread_mutex_destroy(&mon->start_lock);
	pthread_cond_destroy(&mon->start_cond);

	free(mon->state);
	free(mon->philo_cond);
	free(mon);
}

// Take forks (philosopher wants to eat)
void	take_forks(t_monitor *mon, int philo_id)
{
	bool	announce_eating;
	bool	announce_hungry;

	pthread_mutex_lock(&mon->lock);
	if (mon->simulation_end)
	{
		pthread_mutex_unlock(&mon->lock);
		return;
	}
	announce_hungry = true;
	announce_eating = false;
	mon->state[philo_id] = HUNGRY;
	test(mon, philo_id);
	while (mon->state[philo_id] != EATING && !mon->simulation_end)
		pthread_cond_wait(&mon->philo_cond[philo_id], &mon->lock);
	if (!mon->simulation_end && mon->state[philo_id] == EATING)
		announce_eating = true;
	pthread_mutex_unlock(&mon->lock);

	// Print outside the monitor lock to reduce contention and prevent deadlocks
	if (announce_hungry)
		print_status(mon, philo_id, "is hungry");
	if (announce_eating)
	{
		print_status(mon, philo_id, "has taken a fork");
		print_status(mon, philo_id, "has taken a fork");
		print_status(mon, philo_id, "is eating");
	}
}

// Put forks down (philosopher finished eating)
void	put_forks(t_monitor *mon, int philo_id)
{
	int	left;
	int	right;

	pthread_mutex_lock(&mon->lock);
	mon->state[philo_id] = THINKING;
	left = LEFT(philo_id, mon->num_philos);
	right = RIGHT(philo_id, mon->num_philos);
	test(mon, left);
	test(mon, right);
	pthread_mutex_unlock(&mon->lock);
}
