/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glacroix <glacroix@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/14 18:38:57 by glacroix          #+#    #+#             */
/*   Updated: 2023/09/06 13:18:23 by glacroix         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <string.h>
# include <unistd.h>

/*Print Colors----------------------------------------------------------------*/
# define RESET  		"\x1B[0m"
# define RED  			"\x1B[31m"
# define GREEN	  		"\x1B[32m"
# define YELLOW  		"\x1B[33m"
# define BLUE 			"\x1B[34m"
# define MAGENTA 		"\x1B[35m"
# define CYAN  			"\x1B[36m"
# define WHITE  		"\x1B[37m"
# define ORANGE			"\x1b[38;5;209m"

/*Data Structures-------------------------------------------------------------*/
typedef struct t_data {
	long long		time_to_die;
	long long		time_to_eat;
	long long		time_to_sleep;
	long long		start_time;
	int				max_eating_cycles;
	int				philo_died;
	int				nbr_philos;
	int				log;
	int				exit_flag;
	pthread_mutex_t	*forks;
	pthread_mutex_t	ready_set;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	death_mutex;
	pthread_mutex_t	max_eat_mutex;
	pthread_mutex_t	finished_eating_mutex;
	pthread_mutex_t	init_mutex;
}				t_data;

typedef struct t_philo {
	pthread_t	thread;
	long long	start_time;
	long long	finished_eating_time;
	int			id;
	int			ate_count;
	int			ate_enough;
	int			exit_flag;
	t_data		*data;
}				t_philo;

/*Homemade boolean------------------------------------------------------------*/
# define TRUE 1
# define FALSE 0

/*1) Init---------------------------------------------------------------------*/
int			init_args(int argc, char **argv, t_data *data);
int			init_threads(t_data *data);

/*2) Utils--------------------------------------------------------------------*/
int			args_wrong(int argc, char **argv);
int			data_wrong(t_data *data);
int			ft_isdigit(char c);
int			is_number(char *str);
long long	current_time(void);
int			ft_atoi(char *str);
void		ft_sleep(long long time);
void		ft_putstr_fd(char *s, int fd);
size_t		ft_strlen(const char *str);
void		free_memory(t_data *data, t_philo *philo);
int			thread_join(t_data *data, t_philo *philo);

/*3) schedule-----------------------------------------------------------------*/
void		*routine(t_philo *philo);
void		routine_check(t_data *data, t_philo *philo, int *stop);
void		takeforks(t_philo *philo);
void		eat(t_philo *philo);
void		drop_n_sleep(t_philo *philo);
int			philo_died(t_philo *philo);
int			philo_ate_enough(t_philo *philo);
void		routine_stop(t_philo *philo, int *stop);

/*4) Memory control-----------------------------------------------------------*/
void		ft_leaks(void);
int			data_protection(int n, pthread_mutex_t *mutex);

#endif
