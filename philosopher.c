#include "philosopher.h"

# define MAX_ARGS 5

void    init_data(t_philo *philo, long *values, pthread_mutex_t *forks,
            pthread_mutex_t *death)
{
    int *check_death;
    int i;

    i = 0;
    check_death = malloc(sizeof(int));
    *check_death = 0;
    while (i < values[0])
    {
        philo[i].id = i + 1;
        philo[i].num_philo = values[0];
        philo[i].time_to_die = values[1];
        philo[i].time_to_eat = values[2];
        philo[i].time_to_sleep = values[3];
        philo[i].num_must_eat = values[4];
        philo[i].meals_eaten = 0;
        philo[i].last_meal = 0;
        philo[i].left_fork = &forks[i];
        philo[i].right_fork = &forks[(i + 1) % values[0]];
        philo[i].death_mutex = death;
        philo[i].check_death = check_death;
        i++;
    }
    pthread_mutex_init(death, NULL);
}

void see_datas(t_philo *philo)
{
    int i;

    i = 0;
    while (i < philo->num_philo)
    {
        printf("Philosopher %d:\n", philo[i].id);
        printf("  Time to die: %ld\n", philo[i].time_to_die);
        printf("  Time to eat: %ld\n", philo[i].time_to_eat);
        printf("  Time to sleep: %ld\n", philo[i].time_to_sleep);
        printf("  Number of meals to eat: %ld\n", philo[i].num_must_eat);
        printf("  Meals eaten: %d\n", philo[i].meals_eaten);
        printf("  Left fork address: %p\n", (void *)philo[i].left_fork);
        printf("  Right fork address: %p\n", (void *)philo[i].right_fork);
        i++;
    }
}

int main(int argc, char **argv)
{
    t_philo         *philo;
    pthread_mutex_t *forks;
    pthread_mutex_t *death;
    long            values[MAX_ARGS];
    int             i;

    if (!ft_parse_args(ac, av, values))
        return (1);
    philo = malloc(sizeof(t_philo) * values[0]);
    forks = malloc(sizeof(pthread_mutex_t) * values[0]);
    death = malloc(sizeof(pthread_mutex_t));
    if (!philo || !forks || !death)
        return (ft_free_all(philo, forks, death), 1);
    ft_init_data(philo, values, forks, death);
    printf("Data loaded\n");
    see_datas(philo);
}
