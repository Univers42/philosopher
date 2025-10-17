#ifndef PHILO_H
#define PHILO_H

#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include "../minilibft/minilibft.h"

/* time-argument indexes */
typedef enum et_args
{
    TT_DIE,
    TT_EAT,
    TT_SLEEP
}   tt_args;

/* severity/status for argument parsing */
typedef enum e_severity
{
    SEV_OK,
    SEV_WARNING,
    SEV_FATAL
}   t_severity;

typedef struct s_status
{
    int         state;      /* 0 for OK, non-zero for error code if needed */
    t_severity  severity;
}   t_status;

/* general constants */
#define MAX_PHILOS      200
#define RADIX_DEC       10

/* messages */
#define MSG_ERR_PARSE   "./philo <Nphilo> <TT_DIE> <TT_EAT> <TT_SLEEP> [Nmeal]\n" \
                        "Nphilo [1..200]\nTT_* are in ms (>0)\n" \
                        "Nmeal [-1 for infinite, otherwise >=1]"

/* flag bits for parse warnings/errors */
#define ERR_FATAL_PARSE      (1u << 0)
#define WARN_MEAL_UNLIMITED  (1u << 1)
#define ERR_FATAL_MASK       (ERR_FATAL_PARSE)
#define WARN_MASK            (WARN_MEAL_UNLIMITED)

/* shared forward declarations */
struct s_data;

/* philosopher */
typedef struct s_philo
{
    unsigned char   id;
    unsigned long   last_meal;
    int             meals_eaten;
    pthread_mutex_t meal_lock;      /* per-philosopher lock for meal fields */
    pthread_mutex_t *left_fork;
    pthread_mutex_t *right_fork;
    struct s_data   *ctx;           /* shared context */
    pthread_t       thread;
}   t_philo;

/* global context */
typedef struct s_data
{
    unsigned long       args[3];
    unsigned char       n_philo;
    int                 n_meal;         /* -1 means infinite */

    pthread_mutex_t     *forks;         /* array size n_philo */

    pthread_mutex_t     print;          /* print lock */
    pthread_mutex_t     alive_m;        /* protects all_alive */
    pthread_mutex_t     finished_m;     /* protects finished */

    int                 all_alive;      /* 1 while simulation runs */
    int                 finished;       /* 1 when a terminal condition occurs */

    unsigned long       start_time;

    t_philo             *phi;           /* array size n_philo */
}   t_data;

/* parsing */
t_status   parse_args(int argc, char **argv, t_data *data, int *flags);

/* init/cleanup */
int        init_simulation(t_data *ctx);
void       cleanup(t_data *ctx);

/* run */
int        start_threads(t_data *ctx);
void       wait_threads(t_data *ctx);
void      *routine(void *arg);
void      *monitor(void *arg);
void      *monitor_meals(void *arg);

/* actions */
int        take_forks(t_philo *philo);
void       release_forks(t_philo *philo);
void       eat(t_philo *philo);
void       sleep_and_think(t_philo *philo);

/* state management */
int        get_simulation_state(t_data *ctx);
void       set_simulation_state(t_data *ctx, int state);
int        check_philo_death(t_data *ctx, int i);
int        check_philo_ate(t_data *ctx);

/* logging */
void       print_state(t_philo *p, const char *msg);
void       print_state_died(t_philo *p);

#endif