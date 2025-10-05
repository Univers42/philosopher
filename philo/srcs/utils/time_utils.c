#include "philo.h"

// Returns current time in milliseconds
t_time	get_time(void)
{
	struct timeval	time;
	gettimeofday(&time, NULL);
	return ((t_time)time.tv_sec * 1000 + (t_time)time.tv_usec / 1000);
}

t_time	chrono_sum(t_time start, t_time delay)
{
	return (start + delay);
}

t_time	chrono_diff(t_time most_recent)
{
	return (get_time() - most_recent);
}

/**
usleep is not precise, it may overlap
this is a time checking loop instead of raw sleep
@return current sleep duration
*/
t_time	precise_sleep(t_time ms)
{
    t_time	start;
    t_time	elapsed;
    t_time	remaining;

    start = get_time();
    while (1)
    {
        elapsed = get_time() - start;
        if (elapsed >= ms)
            break;
        remaining = ms - elapsed;
        if (remaining > 10)
            usleep(5000);
        else if (remaining > 2)
            usleep(1000);
        else
            usleep(100);
    }
    return (elapsed);
}