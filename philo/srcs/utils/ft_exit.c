#include "philo.h"

bool    ft_exit(const char *str)
{
    ft_putstr_fd("Error : ", 2);
    ft_putrstr_fd(str, 2);
    return (EXIT_FAILURE);
}

