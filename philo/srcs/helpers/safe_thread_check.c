#include "philo.h"

int	check_death(t_philo *ph, int i)
{
	t_ctx *ctx = ph->ctx;

	if (i)
		set_stop(ctx, i);
	return (get_stop(ctx) != 0);
}
