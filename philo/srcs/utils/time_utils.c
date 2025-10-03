#include "philo.h"

uint64_t	current_time()
{
	uint64_t		time;
	struct timeval	cur_time;

	time = 0;
	if (gettimeofday(&cur_time, NULL) == -1)
		ft_exit("Gettimeofday returned - 1\n");
	time = (cur_time.tv_sec * 1000) + (cur_time.tv_usec / 1000);
	return (time);
}

// Get current timestamp in milliseconds
uint64_t	timestamp_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec / 1000);
}


/**
usleep is not precise, it may overlap
this is a time checking loop instead of raw sleep
*/
void	precise_sleep(uint64_t ms)
{
	uint64_t	start;
	uint64_t	elapsed;
	uint64_t	remaining;

	start = timestamp_ms();
	while (1)
	{
		elapsed = timestamp_ms() - start;
		if (elapsed >= ms)
			break;
		remaining = ms - elapsed;
		// Use shorter intervals as we approach the target
		if (remaining > 10)
			usleep(5000); // 5ms when far from target
		else if (remaining > 2)
			usleep(1000); // 1ms when closer
		else
			usleep(100);  // 100μs for final precision
	}
}

void	ft_usleep(uint64_t	ms)
{
	uint64_t	start;

	start = current_time();
	while ((current_time - start) < time_in_ms)
		usleep(time_in_ms / 10);
}

