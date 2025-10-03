#include "philo.h"

/**
usleep is not precise, it may overlap
this is a time checking loop instead of raw sleep
*/
void	precise_sleep(uint64_t ms)
{
	uint64_t	start = timestamp_ms();
	while (timestamp_ms() - start < ms)
		usleep(100); // short nap then check again
}