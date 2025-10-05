#include "philo.h"

t_flag  ft_exit(const char *str)
{
    ft_putstr_fd("Error: ", 2);
    ft_putstr_fd(str, 2);
    return (EXIT_FAILURE);
}

