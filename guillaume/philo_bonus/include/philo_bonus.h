/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glacroix <PGCL>                            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/14 18:38:57 by glacroix          #+#    #+#             */
/*   Updated: 2024/09/11 17:41:28 by glacroix         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_BONUS_H
# define PHILO_BONUS_H

# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <string.h>
# include <semaphore.h>
# include <unistd.h>
# include <signal.h>
#include <fcntl.h>

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
	int				n_philos;
	int				exit_flag;
	sem_t			*forks;
	sem_t			*sem_death;
	sem_t			*sem_data;
	sem_t			*sem_start_time;
	sem_t			*sem_print;
}				t_data;

typedef struct t_philo {
	pid_t		pidc;
	long long	finished_eating_time;
	int			id;
	int			ate_count;
	int			exit_flag;
	t_data		*data;
}				t_philo;

/*Shortcuts-------------------------------------------------------------------*/
# define FALSE 0
# define TRUE 1
# define EXIT_EAT 2
# define EXIT_DEATH 3

/*Functions-------------------------------------------------------------------*/

//base_setup
int			args_wrong(int argc, char **argv);
int			data_wrong(t_data *data);
int			init_semaphore(t_data *data);
int			init_args(int argc, char **argv, t_data *data);

//init
int			init_processes(t_data *data);

//utils
int			ft_isdigit(char c);
void		ft_sleep(long long time, t_philo *philo);
int			is_number(char *str);
long long	current_time(void);
int			ft_atoi(char *str);
size_t		ft_strlen(const char *str);
void		ft_putstr_fd(char *s, int fd);

//routine
void		routine(t_philo *philo);
void		takeforks(t_philo *philo);
void		eat(t_philo *philo);
void		drop_n_sleep(t_philo *philo);

//routine_check
void		routine_stop(t_philo *philo, int *stop);

#endif
