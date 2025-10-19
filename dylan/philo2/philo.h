/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/19 04:58:25 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 06:42:41 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdint.h>
# include <stdbool.h>
# include "minilibft.h"

/* flag bits for parse warnings/errors */
# define ERR_FATAL_PARSE		1
# define WARN_MEAL_UNLIMITED	2
# define ERR_FATAL_MASK			1
# define WARN_MASK				2

/* general constants */
# define MAX_PHILOS				200
# define RADIX_DEC				10

/* debug mode (set via Makefile) */
# ifndef DEBUG_MODE
#  define DEBUG_MODE			0
# endif

# define ENHANCED_OUTPUT		1

/* log levels */
# define LOG_STATE				0
# define LOG_INFO				1
# define LOG_WARNING			2
# define LOG_ERROR				3
# define LOG_DEATH				4

/* debug statistics */
typedef struct s_debug_stats
{
	unsigned long	first_action_time;
	unsigned long	last_action_time;
	unsigned long	death_time;
	unsigned long	death_detected_time;
	int				death_philo_id;
	int				total_actions;
	int				total_forks;
	int				total_meals;
	int				total_sleeps;
	int				total_thinks;
}	t_debug_stats;

/* state codes for simulation */
typedef enum e_sim_state
{
	SIM_STATE_OK = 0,
	SIM_STATE_RUNNING = 1,
	SIM_STATE_COMPLETED = 2,
	SIM_STATE_DEATH = 3,
	SIM_STATE_INFO = 100,
	SIM_STATE_WARNING = 200,
	SIM_STATE_ERROR = 300
}	t_sim_state;

/* time-argument indexes */
typedef enum e_args
{
	TT_DIE,
	TT_EAT,
	TT_SLEEP
}	t_args;

/* severity/status for argument parsing */
typedef enum e_severity
{
	SEV_OK,
	SEV_WARNING,
	SEV_FATAL
}	t_severity;

typedef struct s_status
{
	int			state;
	t_severity	severity;
}	t_status;

/* shared forward declarations */
typedef struct s_data	t_data;

/* philosopher */
typedef struct s_philo
{
	unsigned char	id;
	unsigned long	last_meal;
	int				meals_eaten;
	pthread_mutex_t	meal_lock;		/* per-philosopher lock for meal fields */
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	struct s_data	*ctx;			/* shared context */
	pthread_t		thread;
}	t_philo;

/* global context */
typedef struct s_data
{
	unsigned long		args[3];
	unsigned char		n_philo;
	int					n_meal;
	pthread_mutex_t		*forks;
	pthread_mutex_t		print;
	pthread_mutex_t		alive_m;
	pthread_mutex_t		finished_m;
	int					all_alive;
	int					finished;
	unsigned long		start_time;
	t_philo				*phi;
	t_sim_state			sim_state;
	t_debug_stats		debug_stats;
}	t_data;

/* parsing */
t_status	parse_args(int argc, char **argv, t_data *data, int *flags);

/* init/cleanup */
int			init_simulation(t_data *ctx);
void		cleanup(t_data *ctx);

/* run */
int			start_threads(t_data *ctx);
void		wait_threads(t_data *ctx);
void		*routine(void *arg);
void		*monitor(void *arg);
void		*monitor_meals(void *arg);

/* actions */
int			take_forks(t_philo *philo);
void		release_forks(t_philo *philo);
void		eat(t_philo *philo);
void		sleep_and_think(t_philo *philo);

/* state management */
int			get_simulation_state(t_data *ctx);
void		set_simulation_state(t_data *ctx, int state);
int			check_philo_death(t_data *ctx, int i);
int			check_philo_ate(t_data *ctx);

/* logging */
void		print_state(t_philo *p, const char *msg);
void		print_state_died(t_philo *p);
void		log_debug(t_philo *p, int level, const char *msg);
void		print_debug_summary(t_data *ctx);
void		print_debug_header(t_data *ctx);

#endif