/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 12:47:11 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/06 19:03:05 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHER_H
# define PHILOSOPHER_H
# include <pthread.h>
# include <errno.h>
# include <string.h>
# include <unistd.h>
# include <sys/resource.h>
# include <stdio.h>
# include <sched.h>
# include <stddef.h>
# include <sys/time.h>
# include <stdlib.h>

# define N 200
# define THINKING 2
# define HUNGRY 3
# define EATING 1
# define SLEEPING 0
# define LEFT (phnum + 4) % N
# define RIGHT (phnum + 1) % N

// Helper macro to cast opaque attr to internal structure
#define ATTR_TO_INTERNAL(attr) ((t_pthread_attr*)(attr))

// Default values
#define DEFAULT_STACK_SIZE (2 * 1024 * 1024)
#define DEFAULT_GUARD_SIZE 4096

typedef struct s_philo	t_philo;

typedef struct s_sim
{
	int				num_philos;
	pthread_mutex_t	*forks;
	pthread_mutex_t	print;
	pthread_mutex_t	state;
	volatile int	stop;
	int				finished;
	long			start_ms;
	long			start_us;        // high-resolution start time (microseconds)
	int				meals_required;
	// Waiter: allow up to N-1 philosophers to try eating at once
	int				slots;
	pthread_mutex_t	slot;
	volatile int	urgent_id; // -1 means no priority, else philosopher id to favor
	// Fairness helpers
	int				next_id;   // round-robin grant pointer (1..num_philos)
	int				*pending;  // length num_philos, 1 when philo is waiting for a slot
	long			*pending_since; // ms when pending==1 (per philo)
	struct s_philo	*philos;
#ifdef FORK_CHECK
	int				*fork_owner;     // owner philosopher id or -1
#endif
}					t_sim;

typedef struct s_philo
{
	int				id;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				meals_required;
	int				left_fork;
	int				right_fork;
	long			last_meal_time;
	long			last_meal_us;    // high-resolution last meal (microseconds)
	int				meals_eaten;
	int				state;
	void			*thread;
	void			(*init)(int *values, struct s_philo *philos);
	t_sim			*sim;
	pthread_attr_t	attr;   // per-philosopher scheduling attributes (custom utils)
}                   t_philo;

typedef struct s_pthread_attr
{
	pthread_attr_t		system_attr;	//for compatibility
	int					detachstate;
	size_t				guardsize;
	void				*stackaddr;
	int					scope;
	int					policy;
	size_t				stacksize;
	struct sched_param	schedparam;
	int					inheritsched;
}						t_pthread_attr;

int		conv_pos_int(char *str);
void	argcheck(int argc, char **argv, int *values);

// pthread-attr wrappers (safe wrappers around POSIX pthread_attr_* APIs)
int		ft_pthread_attr_init(pthread_attr_t *attr);
int		ft_pthread_attr_destroy(pthread_attr_t *attr);
int		ft_pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
int		ft_pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
int		ft_pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize);
int		ft_pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
int		ft_pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
int		ft_pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr);
int		ft_pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr);
int		ft_pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);
int		ft_pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize);
int		ft_pthread_attr_setscope(pthread_attr_t *attr, int scope);
int		ft_pthread_attr_getscope(const pthread_attr_t *attr, int *scope);
int		ft_pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
int		ft_pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy);
int		ft_pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param);
int		ft_pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param);
int		ft_pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched);
int		ft_pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched);
int		ft_pthread_attr_validate(const pthread_attr_t *attr);
void	ft_pthread_attr_print(const pthread_attr_t *attr);
void	analyze_pthread_attr(void);
int		ft_pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);

// Optional build: make DEATH_CHECK=1
#ifdef DEATH_CHECK
# define DEATH_TOLERANCE_MS 10
#endif

// timing/logging helpers
long	get_time_ms(void);
long	get_time_us(void);
void	ft_usleep_ms(long ms, t_sim *sim);
void	log_status(t_sim *sim, int id, const char *msg);
void	death_timing_log(int id, long expected_ms, long actual_ms,
			long delta_ms);
int		sim_get_stop(t_sim *sim);
void	sim_set_stop(t_sim *sim);
void	philo_init(int *values, t_philo *philos, t_sim *sim);
void	*philo_routine(void *arg);
void	waiter_give_slot(t_sim *sim);
void	waiter_take_slot(t_philo *ph);
void	report_fork_violation(t_sim *sim, int fork_idx, int current, int attempted);
void	mark_fork_acquire(t_philo *p, int fork_idx);
void	mark_fork_release(t_philo *p, int fork_idx);
void	lock_forks(t_philo *p);
void	unlock_forks(t_philo *p);
int		best_pending_by_priority(t_sim *sim);

// Debug logger (enabled with -DDEBUG)
void	dbg(t_sim *sim, int id, const char *tag, const char *fmt, ...);
#ifdef DEBUG
# define DBG(sim, id, tag, fmt, ...) dbg((sim), (id), (tag), (fmt), ##__VA_ARGS__)
#else
# define DBG(sim, id, tag, fmt, ...) ((void)0)
#endif

# endif