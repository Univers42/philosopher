#include "philo.h"

// we finally can change this and just do a machine state to compress value
static bool	parse_args(t_args *args, int argc, const char **argv)
{
	if (argc < 5 || argc > 6)
		return (false);
	args->nfork = ft_strto_uchar(argv[1], NULL, RADIX_DEC);
	args->time_to_die = ft_strtou64(argv[2], NULL, RADIX_DEC);
	args->time_to_eat = ft_strtou64(argv[3], NULL, RADIX_DEC);
	args->time_to_sleep = ft_strtou64(argv[4], NULL, RADIX_DEC);
	if (argv[5])
		args->nb_dish = ft_strto_uint(argv[5], NULL, RADIX_DEC);
	else
		args->nb_dish = 0;
	if (args->nfork == 0 || args->nfork > MAX_PHILOS)
		return (false);
	if (args->time_to_eat < MIN_MS || args->time_to_die < MIN_MS)
		|| args->time_to_sleep < MIN_MS)
		return (false);
	return (true);
}
