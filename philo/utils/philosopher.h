/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosopher.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 12:47:11 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/05 12:53:42 by dlesieur         ###   ########.fr       */
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

# define N 10
# define THINKING 2
# define HUNGRY 3
# define EATING 1
# define SLEEPING 0
# define LEFT (phnum + 4) % N
# define RIGHT (phnum + 1) % N
// Helper macro to cast opaque attr to internal structure
#define ATTR_TO_INTERNAL(attr) ((t_pthread_attr*)(attr))

// Default values
#define DEFAULT_STACK_SIZE (2 * 1024 * 1024)  // 2MB
#define DEFAULT_GUARD_SIZE 4096                // Usually page size

typedef struct s_philo
{
	int             id;
	int             time_to_die;
	int             time_to_eat;
	int             time_to_sleep;
	int             meals_required;
	int             left_fork;
	int             right_fork;
	long            last_meal_time;
	int             meals_eaten;
	int             state;
	void            *thread;
	void            (*init)(int *values, t_philo *philos);
}                   t_philo;

typedef struct s_pthread_attr
{
	pthread_attr_t		system_attr;		//for compatibility
	int					detachstate;
	size_t				guardsize;
	void				*stackaddr;
	int					scope;
	int					policy;
	size_t				stacksize;
	struct sched_param	schedparam;
	int					inheritsched;
}						t_pthread_attr;

int     conv_pos_int(char *str);
void    argcheck(int argc, char **argv, int *values);

# endif