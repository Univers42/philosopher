#ifndef PHILO_H
#define PHILO_H

#include <pthread.h>
#include <stdint.h>
#include "../minilibft/minilibft.h"

/*___TEXTS_TO_PRINT___*/
# define ARGS_KO 	"Invalid Arguments\n"
# define PRINTER 	"\033[1;30m%ld ms \033[1;31m%d %s\n"
# define FORK_TAKEN "\033[1;33mhas taken a fork"
# define EATING 	"\033[1;32mis eating"
# define SLEEPING 	"\033[1;34mis sleeping"
# define THINKING 	"\033[1;36mis thinking"
# define DIED		"\033[1;31mdied"

/*___OTHERS___*/
# define MALLOC_KO 			0
# define MUTEX_OK 			1
# define TRUE				1
# define FALSE				0
# define LOCKED				0
# define UNLOCKED			1
# define MAX_ARGS       	5
# define PRINT				1
# define NO_PRINT			0

# define PHILO_C            0
# define T_DIE              1
# define T_EAT              2
# define T_SLEEP            3
# define MEALS_C            4

# define DEBUG_MODE			0  // Set to 1 to enable debug output for death cases only
# define MAX_DEATH_DELAY	10

// Reasonable upper bound to avoid non-terminating simulations (in ms)
# define MAX_TIME_MS        1000000000L  // 1e9 ms ~ 11.5 days
# define DEFAULT_TIMEOUT_MS 60000L       // 60 seconds runtime cap
// Small grace period to absorb scheduling/printing jitter in death detection (ms)
# define DEATH_EPS_MS       20
# define URGENCY_MARGIN_MS  10

typedef struct s_data	 t_data;
typedef pthread_mutex_t t_mutex;
typedef uint64_t		 t_time;
typedef unsigned int	 u_seconds_t;

typedef struct s_philo {
	int			id;
	int			l_fork;
	int			r_fork;
	long		last_meal;
	long		max_time_to_eat;
	int			is_eating;
	t_mutex		eating_mutex;
	int			eat_count;
	pthread_t	thread_id;
	t_data		*c;
}	t_philo;

typedef struct s_data {
	char		**argv;
	int			argc;
	t_mutex		*forks;
	t_mutex		m_finish;
	t_mutex		printing;
	t_mutex		death;
	long		program_start_time;
	long		args[MAX_ARGS];
	int			finish;
	int			debug;
	t_mutex		waiter;
	int			waiter_slots;
	int			waiter_capacity;
	t_philo		*philos;
	t_time		timeout_ms;    // runtime timeout
}	t_data;

void	unlock(t_philo *p);
int		lock(t_philo *p);      // unchanged
int		get_fork(t_philo *p);  // changed: now returns 1 on success, 0 otherwise
void	eat(t_philo *p);
void	leave_fork(t_philo *p);

void	*set_death_and_exit(t_data *c, int i, int print_dead);
int		is_dead(t_philo *p);
int		done(t_philo *p);
int		had_all_meals(t_data *c, int i);

int		print_game(t_philo *phils, char *str);
void	debug_print(t_data *c, int philo_id, const char *format, ...);
void	print_one(t_time time);
void	get_value_safe(pthread_mutex_t *mutex, void *target,
		const void *source, size_t n);
void	set_value_safe(pthread_mutex_t *mutex, void *target,
		const void *source, size_t n);

int		init_mutex(t_data *c);
int		init_checker(t_data *c);
int		init_philosopher(t_data *c);
int		init_args(t_data *c, int argc, char **argv);

void	*death_checker(void *p);
void	*meals_checker(void *p);
void	*timeout_checker(void *p); // added
void	*dinner(void *p);
#endif // PHILO_H
