#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <sys/time.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <string.h>

typedef unsigned long long t_time;

typedef struct s_philo
{
    int             id;
    int             total_philos;
    int             must_eat;
    int             meals_eaten;
    int             *check_dead;
    int             is_eating;
    t_time          time_to_die;
    t_time          time_to_eat;
    t_time          time_to_sleep;
    t_time          start_time;
    t_time          last_meal;
    pthread_t       thread;
    pthread_mutex_t *left_fork;
    pthread_mutex_t *right_fork;
    pthread_mutex_t *death;
    int             is_dead;
}t_philo;

#endif
