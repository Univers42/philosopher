#include "philo.h"

int	check_death(t_philo *ph, int i)
{
	pthread_mutex_lock(&ph->pa->dead);
}

