/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 19:43:42 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/06 00:18:24 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

#include <stddef.h>
#include <stdint.h>
#include "conv.h"

#define RADIX_DEC 10

typedef uint64_t t_time;
//CTYPE
int		ft_isspace(int c);
int		ft_isdigit(int c);
int		ft_isalpha(int c);
int		ft_is_number(const char *str);

//MEMORY
void	*ft_memcpy(void *dst, const void *src, size_t n);
void	*ft_calloc(size_t nmemb, size_t size);
void	ft_swap(void *a, void *b, size_t n);

//STDIO
void	ft_putchar_fd(const char s, int fd);
void	ft_putstr_fd(const char *s, int fd);
void	ft_putnbr_fd(size_t nb, int fd);

//STRING
size_t	ft_strlen(const char *s);
void	ft_tolower(char *c);

//SYS/TIME
t_time	cur_time(void);
t_time	time_dif(t_time since);
t_time	ft_positive_offset(t_time start, t_time offset);
t_time	ft_neg_offset(t_time offset);
t_time	ft_usleep(t_time wait);
t_time	ft_precise_usleep(t_time wait);

#endif