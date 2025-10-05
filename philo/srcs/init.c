#include "philo.h"

void    *set_death_and_exit(t_data *c, int i, int print_dead)
{
    if  (print_dead)
    {

    }
    else
    {
        pthread_mutex_lock(&c->m_finish);
        c->finish = TRUE;
        pthread_mutex_unlock(&c->m_finish);
    }
    return (NULL);
}

