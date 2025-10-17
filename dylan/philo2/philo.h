#ifndef PHILO_H
#define PHILO_H

#include <pthread.h>
#include <stdint.h>
#include "../minilibft/minilibft.h"


typedef struct s_data		t_data;
typedef pthread_mutex_t		t_mutex;
typedef uint64_t			t_time;
typedef unsigned int		t_seconds;

/*___TEXTS_TO_PRINT___*/
#define ARGS_KO			"Invalid Arguments\n"
#define PRINTER			"\033[1;30m%ld ms \033[1;31m%d %s\n"
#define FORK_TAKEN		"\033[1;33mhas taken a fork"
#define EATING			"\033[1;32mis eating"
#define SLEEPING		"\033[1;34mis sleeping"
#define THINKING		"\033[1;36mis thinking"
#define DIED			"\033[1;31mdied"
#define MSG_ERR_PARSE	"./philo <Nphilo> <TT_DIE> <TT_EAT> <TT_SLEEP> <?Nmeal>" \
						"Nphilo [0,200]\nTT_DIE [0;MAX_LONG]"                    \
						"TT_EAT [0;MAX_LONG]\n TT_SLEEP [0;MAX_LONG]"            \
						"TT_NMEAL [-1;MAX_INT] {-1 = infinite explicit}"
#define MSG_DONE		"all philo have survived" // template
#define MSG_DEATH		"philo has died"		   // template
#define MSG_ACTION		"taken the lfork"	   // template

/*___OTHERS___*/
#define MAX_PHILOS				200
#define MAX_ARGS				5
#define TIME_ARGS				3
#define TRUE					1
#define FALSE					0
#define MONITORING				TRUE
#define MAX_DEATH_DELAY			10
#define DEATH_EPS_MS			20
#define MAX_TIME_MS				1000000000UL // 1e9 ms ~ 11.5 days
#define DEFAULT_TIMEOUT_MS		60000 // 60 seconds runtime cap
#define URGENCY_MARGIN_MS		10

// Error/Warning flags
#define ERR_FATAL_PARSE			0x01
#define ERR_FATAL_INIT			0x02
#define WARN_MEAL_UNLIMITED		0x10

#define ERR_FATAL_MASK		(ERR_FATAL_PARSE | ERR_FATAL_INIT)
#define WARN_MASK			(WARN_MEAL_UNLIMITED)

# ifndef DEBUG
#  define DEBUG 0 // Set to 1 to enable debug output for death cases only
# endif

/**
 * categorize statuses (success, error, warning, info) with clear numeric range
 * easily extendible
 * improve readability
 * integrate with logging funcitons to print meaningful based on state
 * 
 * For even better debuggign it's best to map them as I first did
 * with init dispatcher
 * 
 */
typedef enum e_state
{
	// Success
	STATE_OK = 0,
	STATE_OK_MUTEX,
	// Errors (100-199)
	STATE_ERR_BASE = 100,
	STATE_PARSE_ERR,
	STATE_MALLOC_ERR,
	STATE_INIT_ERR,
	STATE_THREAD_ERR,
	STATE_CLEANUP_ERR,
	STATE_ERR_FORK,

	// Warnings (200-299)
	STATE_WARN_BASE = 200,
	STATE_MEAL_WARN,
	// ...add more warnings...
	
	// Infos (300-399)
	STATE_INFO_BASE = 300,
	STATE_MUTEX_LOCKED,
	STATE_MUTEX_UNLOCKED,
	STATE_PRINTING,
	STATE_NOT_PRINTING,
	STATE_TIMESTAMP,
	STATE_CTX,//...
	// ...add more infos...

	STATE_MAX
}	t_state;

/**
 * Even thoug the majority is doing that we should not
 * share context between two structures of meta data
 * But I don't know it that mean that we could share them but with security
 * using correclty the mutexes or if they are just prohibited.
 * This is something that I should deepen thuroughout the project.
 */
typedef struct s_philo
{
	unsigned char	id;
	int				l_fork;
	int				r_fork;
	int64_t			last_meal;
	int				meals_eaten;
	int				max_time_to_eat;
	bool			is_eating;
	t_mutex			*left_fork;
	t_mutex			*right_fork;
	t_mutex			*meal_lock;
	t_data			*ctx; // shared context
	pthread_t		thread;
}	t_philo;

typedef struct s_data
{
	t_time			args[TIME_ARGS];
	int				must_eat;
	t_mutex			*forks;
	int				alive;
	t_mutex			m_finish; //
	t_mutex			print; // print_lock
	t_mutex			death;	  // finish lock
	t_mutex			waiter;	  // alive lock
	t_time			start_time;
	t_time			timeout_ms;
	int 			waiter_slots;
	int 			waiter_capacity;
	unsigned char	n_philo;
	int				n_meal;
	bool			finished;
	bool			debug;
	int				all_alive;
	t_philo			*phi; // shared context
}	t_data;

typedef enum et_args
{
	TT_DIE,
	TT_EAT,
	TT_SLEEP
}	tt_args;

typedef enum e_severity
{
	SEV_OK,
	SEV_WARNING,
	SEV_FATAL
}	t_severity;

typedef struct s_status
{
	t_state state;
	t_severity severity;
}	t_status;


static inline const char **init_dispatcher(void)
{
	static const char *msgs[STATE_MAX] = {
		[STATE_PARSE_ERR] = MSG_ERR_PARSE,
		[STATE_DONE] = MSG_DONE,
		[STATE_DEATH] = MSG_DEATH,
		[STATE_ACTION] = MSG_ACTION};
	return (msgs);
}

t_status	parse_args(int argc, char **argv, t_data *data, int *flags);
t_state		log(t_philo *p, t_state state);

#endif