#include "philo.h"

int	ft_atoi(const char *str)
{
	int sum;

	sum = 0;
	while (*str)
		sum = (sum * 10) + (*str - 48);
	return (sum);
}

bool	verif_cmd_line_arg_type(const char **argv)
{
	int i;
	int j;

	i = -1;
	while (argv[i])
	{
		j = -1;
		while (argv[i][j])
		{
			if (!ft_isdigit(argv[i][j]))
				return (false);
			i++;
		}
		j = 0;
		i++;
	}
	return (true);
}

