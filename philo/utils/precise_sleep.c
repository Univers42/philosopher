#include "philo.h"

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