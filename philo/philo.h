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

#define MSG_ARGS_KO   		"Invalid Arguments\n"
#define MSG_PRINTER   		"\e[1;30m%ld ms \e[1;31m%d %s\n"
#define MSG_FORK_TAKEN 		"\e[1;33mhas taken a fork"
#define MSG_EATING    		"\e[1;32mis eating"
#define MSG_SLEEPING  		"\e[1;34mis sleeping"
#define MSG_THINKING  		"\e[1;36mis thinking"
#define MSG_DIED      		"\e[1;31mdied"

# ifndef DEBUG
#  define DEBUG				0 // enable output for death case only
# endif

# define MAX_DEATH_DELAY	10

typedef pthread_mutex_t	t_mutex;
typedef int				t_flag;
typedef uint64_t		t_time;
typedef unsigned int	t_qty;
typedef int				t_counter;

typedef enum _ebool 
{
	FALSE,
	TRUE
}	t_bool;

typedef enum e_mutex_state
{
	LOCKED,
	UNLOCKED
}	t_mutex_state;

typedef enum e_print
{
	NO_PRINT,
	PRINT
}	t_print;

typedef enum e_status
{
	ST_OK,
	ST_ERR_MALLOC,
}	t_status;

typedef enum e_arg
{
	PHILO_C,
	TTDIE,
	TTEAT,
	TTSLEEP,
	MEALS_C
}	t_arg;

// Philosopher states
typedef enum e_philo_state
{
	THINKING,
	HUNGRY,
	EATING
}	t_philo_state;

typedef struct s_ctx
{
    // Config
	uint8_t			total;			// number of philosophers
	t_time			die;   			// time to die
    t_time			eat;   			// time to eat
    t_time			sleep; 			// time to sleep
    t_qty			m_eat;
    // Timing
    t_time		start_t;   			// simulation start timestamp
    // Shared resources
    t_mutex			*forks;     	// array of fork mutexes
    t_mutex			write_mutex;
    t_mutex			dead;
    t_mutex			time_eat;
    t_mutex			finish;
	// Start barrier
    t_qty			nb_p_finish;    // philosophers who reached m_eat
	t_flag			stop;           // stop flag
    pthread_t		monitor_thread; // monitor thread handle
    t_mutex			start_lock;
    pthread_cond_t  start_cond;
    t_flag			started;
	t_flag			debug;
}   t_ctx;


typedef struct s_philo
{
    uint8_t			id;             // Philosopher ID
    pthread_t		thread_id;      // Thread handle
    t_mutex			*lf;            // Left fork
    t_mutex			*rf;            // Right fork
    t_mutex			eating_mutex;   // Protect local state
    t_time			last_meal;      // Last meal timestamp
    t_counter		eat_count;      // Meals eaten
    t_ctx			*ctx;           // Pointer to shared context
}   t_philo;

// MONITOR HELPERS
t_monitor		*monitor_init(t_args args);
void			monitor_destroy(t_monitor *mon);
void			activity(t_philo *ph);
t_time			current_time(void);

// PRINTERS
int				check_death(t_philo *ph, int i);
void			write_status(const char *str, t_philo *ph);
void			print_status(t_monitor *mon, int philo_id, const char *status);

// UTILS
size_t			ft_strlen(const char *s);
void			ft_putstr_fd(char *s, int fd);
void			ft_usleep(t_time ms);
t_flag			ft_exit(const char *str);
t_uint8			ft_strto_uchar(const char *nptr, char **endptr, int base);
t_time			ft_strtou64(const char *nptr, char **endptr, int base);
unsigned int	ft_strto_uint(const char *nptr, char **endptr, int base);
void			precise_sleep(t_time ms);
t_time			timestamp_ms(void);

// HELPERS
void			take_forks(t_monitor *mon, int philo_id);
void			put_forks(t_monitor *mon, int philo_id);

#endif