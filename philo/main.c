#include "philo.h"

static bool	parse_args(t_args *args, int argc, const char **argv)
{
	if (argc < 5 || argc > 6)
		return (false);
	args->nfork = ft_strto_uchar(argv[1], NULL, 10);
	args->time_to_die = ft_strtou64(argv[2], NULL, 10);
	args->time_to_eat = ft_strtou64(argv[3], NULL, 10);
	args->time_to_sleep = ft_strtou64(argv[4], NULL, 10);
	if (argv[5])
		args->nb_dish = ft_strto_uint(argv[5], NULL, 10);
	else
		args->nb_dish = 0;
	if (args->nfork == 0 || args->nfork > 200)
		return (false);
	if (args->time_to_eat < 60 || args->time_to_die < 60 
		|| args->time_to_sleep < 60)
		return (false);
	return (true);
}

static bool	all_philosophers_fed(t_monitor *mon, t_philosopher *philos)
{
	int	i;

	if (mon->args.nb_dish == 0)
		return (false);
	pthread_mutex_lock(&mon->lock);
	i = -1;
	while (++i < mon->num_philos)
	{
		if (philos[i].meals_eaten < mon->args.nb_dish)
		{
			pthread_mutex_unlock(&mon->lock);
			return (false);
		}
	}
	pthread_mutex_unlock(&mon->lock);
	return (true);
}

static bool	check_death(t_monitor *mon, t_philosopher *philos)
{
	int			i;
	uint64_t	now;
	uint64_t	time_since_meal;
	int			died_id;

	pthread_mutex_lock(&mon->lock);
	now = timestamp_ms();
	died_id = -1;
	i = -1;
	while (++i < mon->num_philos)
	{
		time_since_meal = now - philos[i].last_meal;
		if (time_since_meal > mon->args.time_to_die)
		{
			mon->simulation_end = true;
			died_id = i;
			break;
		}
	}
	pthread_mutex_unlock(&mon->lock);
	if (died_id != -1)
	{
		print_status(mon, died_id, "died");
		return (true);
	}
	return (false);
}

static void	*philosopher_routine(void *arg)
{
	t_philosopher	*philo;
	t_monitor		*mon;
	bool			should_exit;

	philo = (t_philosopher *)arg;
	mon = philo->monitor;

	// Wait for global start
	pthread_mutex_lock(&mon->start_lock);
	while (!mon->started)
		pthread_cond_wait(&mon->start_cond, &mon->start_lock);
	pthread_mutex_unlock(&mon->start_lock);

	// Initialize last_meal at start
	pthread_mutex_lock(&mon->lock);
	philo->last_meal = mon->start_time;
	pthread_mutex_unlock(&mon->lock);

	// Stagger just a bit to reduce contention
	if (philo->id % 2 == 1)
		usleep(100);

	should_exit = false;
	while (!should_exit)
	{
		// Check termination conditions
		pthread_mutex_lock(&mon->lock);
		should_exit = mon->simulation_end
			|| (mon->args.nb_dish > 0 && philo->meals_eaten >= mon->args.nb_dish);
		pthread_mutex_unlock(&mon->lock);
		if (should_exit)
			break;

		print_status(mon, philo->id, "is thinking");

		take_forks(mon, philo->id);

		// Check again after forks
		pthread_mutex_lock(&mon->lock);
		if (mon->simulation_end)
		{
			pthread_mutex_unlock(&mon->lock);
			break;
		}
		philo->last_meal = timestamp_ms();
		pthread_mutex_unlock(&mon->lock);

		precise_sleep(mon->args.time_to_eat);

		pthread_mutex_lock(&mon->lock);
		philo->meals_eaten++;
		pthread_mutex_unlock(&mon->lock);

		put_forks(mon, philo->id);

		print_status(mon, philo->id, "is sleeping");
		precise_sleep(mon->args.time_to_sleep);
	}
	return (NULL);
}

static int	run_simulation(t_monitor *mon)
{
	pthread_t		*threads;
	t_philosopher	*philos;
	int				i;
	bool			ended;
	uint64_t		start;

	threads = malloc(sizeof(pthread_t) * mon->num_philos);
	philos = malloc(sizeof(t_philosopher) * mon->num_philos);
	if (!threads || !philos)
		return (EXIT_FAILURE);

	// Compute common start time and pre-initialize last_meal for all
	start = timestamp_ms();
	pthread_mutex_lock(&mon->lock);
	mon->start_time = start;
	pthread_mutex_unlock(&mon->lock);

	// Prepare philosopher structs and set last_meal to start (prevent 0-diffs)
	i = -1;
	while (++i < mon->num_philos)
	{
		philos[i].id = i;
		philos[i].monitor = mon;
		philos[i].last_meal = start;     // critical to avoid immediate deaths
		philos[i].meals_eaten = 0;
	}

	// Create all threads
	i = -1;
	while (++i < mon->num_philos)
	{
		if (pthread_create(&threads[i], NULL, philosopher_routine, &philos[i]) != 0)
		{
			pthread_mutex_lock(&mon->lock);
			mon->simulation_end = true;
			pthread_mutex_unlock(&mon->lock);
			break;
		}
	}

	// Arm the start barrier
	pthread_mutex_lock(&mon->start_lock);
	mon->started = true;
	pthread_cond_broadcast(&mon->start_cond);
	pthread_mutex_unlock(&mon->start_lock);

	// Wait until started before monitor loop
	// (cheap check: at this point it's already true, but keeps semantics clear)
	pthread_mutex_lock(&mon->start_lock);
	while (!mon->started)
		pthread_cond_wait(&mon->start_cond, &mon->start_lock);
	pthread_mutex_unlock(&mon->start_lock);

	// Monitor loop (protect simulation_end access)
	for (;;)
	{
		pthread_mutex_lock(&mon->lock);
		ended = mon->simulation_end;
		pthread_mutex_unlock(&mon->lock);
		if (ended)
			break;

		usleep(1000);
		if (check_death(mon, philos))
			break;
		if (all_philosophers_fed(mon, philos))
		{
			pthread_mutex_lock(&mon->lock);
			mon->simulation_end = true;
			pthread_mutex_unlock(&mon->lock);
			break;
		}
	}

	// Wake up all waiting philosophers
	pthread_mutex_lock(&mon->lock);
	i = -1;
	while (++i < mon->num_philos)
		pthread_cond_broadcast(&mon->philo_cond[i]);
	pthread_mutex_unlock(&mon->lock);

	// Join all threads
	i = -1;
	while (++i < mon->num_philos)
		pthread_join(threads[i], NULL);

	free(threads);
	free(philos);
	return (EXIT_SUCCESS);
}

int	main(int argc, const char **argv)
{
	t_args		args;
	t_monitor	*mon;
	int			ret;

	if (!parse_args(&args, argc, argv))
	{
		printf("Error: Invalid arguments\n");
		printf("Usage: %s number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n", argv[0]);
		return (EXIT_FAILURE);
	}
	mon = monitor_init(args);
	if (!mon)
		return (EXIT_FAILURE);
	ret = run_simulation(mon);
	monitor_destroy(mon);
	return (ret);
}