#ifndef PHILOSOPHER_H
# define PHILOSOPHER_H

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>

// Philosopher states
typedef enum e_philo_state {
	THINKING,
	HUNGRY,
	EATING
} t_philo_state;

typedef struct s_args
{
	unsigned char	nfork;				// 0 <= value <= 200
	uint64_t		time_to_die;		// if <= 60ms ERROR
	uint64_t		time_to_eat;		// if <= 60ms ERROR
	uint64_t		time_to_sleep;		// if <= 60ms ERROR
	unsigned int	nb_dish;
}	t_args;

// Monitor structure for synchronization
typedef struct s_monitor {
	t_philo_state	*state;				// Array of philosopher states
	pthread_cond_t	*philo_cond;		// Array of condition variables
	pthread_mutex_t	lock;				// Mutex for the monitor
	int				num_philos;			// Number of philosophers
	t_args			args;				// Simulation parameters
	uint64_t		start_time;			// Simulation start time
	bool			simulation_end;		// Flag to stop simulation

	// New: dedicated print lock to reduce contention while printing
	pthread_mutex_t	print_lock;

	// New: start barrier so all threads start together
	pthread_mutex_t	start_lock;
	pthread_cond_t	start_cond;
	bool			started;
	bool			print_stopped; // stop printing after a death
}	t_monitor;

// Philosopher thread data
typedef struct s_philosopher {
	int				id;					// Philosopher ID (0-based)
	t_monitor		*monitor;			// Pointer to monitor
	uint64_t		last_meal;			// Timestamp of last meal
	unsigned int	meals_eaten;		// Number of meals eaten
}	t_philosopher;

// Function prototypes
unsigned char	ft_strto_uchar(const char *nptr, char **endptr, int base);
uint64_t		ft_strtou64(const char *nptr, char **endptr, int base);
unsigned int	ft_strto_uint(const char *nptr, char **endptr, int base);

// Monitor functions
t_monitor		*monitor_init(t_args args);
void			monitor_destroy(t_monitor *mon);
void			take_forks(t_monitor *mon, int philo_id);
void			put_forks(t_monitor *mon, int philo_id);

// Utility functions
uint64_t		timestamp_ms(void);
void			precise_sleep(uint64_t ms);
void			print_status(t_monitor *mon, int philo_id, const char *status);

#endif