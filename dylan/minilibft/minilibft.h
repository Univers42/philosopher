/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minilibft.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 18:49:15 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 06:41:38 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINILIBFT_H
# define MINILIBFT_H

# include <stddef.h>
# include <stdint.h>
# include "conv.h"

# define RADIX_DEC 10
# define TRUE 1
# define FALSE 0

# define PRECISE_COARSE_NS  2000000ULL
# define PRECISE_MARGIN_NS  200000ULL
# define PRECISE_SPIN_NS    50000ULL

typedef struct s_precise_stats
{
	double		estimate;
	double		mean;
	double		m2;
	uint64_t	count;
}	t_precise_stats;

static inline t_precise_stats	*get_precise_stats(void)
{
	static t_precise_stats	g_precise = {5e-3, 5e-3, 0.0, 1};

	return (&g_precise);
}

typedef uint64_t    t_time;

//CTYPE
int			ft_isspace(int c);
int			ft_isdigit(int c);
int			ft_isalpha(int c);
int			ft_is_number(const char *str);

//MEMORY
void		*ft_memcpy(void *dst, const void *src, size_t n);
void		*ft_calloc(size_t nmemb, size_t size);
void		ft_swap(void *a, void *b, size_t n);

//STDIO
void		ft_putchar_fd(const char s, int fd);
void		ft_putstr_fd(const char *s, int fd);
void		ft_putnbr_fd(size_t nb, int fd);

//STRING
size_t		ft_strlen(const char *s);
void		ft_tolower(char *c);
int			ft_strncmp(const char *msg, const char *s2, size_t n);
//SYS/TIME
t_time		cur_time(void);
t_time		get_time(void);
t_time		time_dif(t_time since);
t_time		ft_positive_offset(t_time start, t_time offset);
t_time		ft_neg_offset(t_time offset);
t_time		ft_precise_usleep(t_time wait);
void		ft_usleep(unsigned long time_in_ms);
uint64_t	now_ns(void);
uint64_t	ft_precise_sleep(uint64_t duration_ns);
uint64_t	elapsed_time(uint64_t time);

bool		within_range(unsigned short min, unsigned short max, unsigned short v);

#endif