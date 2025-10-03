#include "philo.h"

// Get current timestamp in milliseconds
uint64_t	timestamp_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec / 1000);
}

// Print philosopher status with timestamp, guarded by print_lock only
void	print_status(t_monitor *mon, int philo_id, const char *status)
{
	uint64_t	elapsed;
	bool		is_death;

	elapsed = timestamp_ms() - mon->start_time;
	is_death = (strcmp(status, "died") == 0);

	pthread_mutex_lock(&mon->print_lock);
	if (mon->print_stopped && !is_death)
	{
		pthread_mutex_unlock(&mon->print_lock);
		return;
	}
	if (is_death)
		mon->print_stopped = true;
	printf("%llu %d %s\n", (unsigned long long)elapsed, philo_id + 1, status);
	pthread_mutex_unlock(&mon->print_lock);
}
