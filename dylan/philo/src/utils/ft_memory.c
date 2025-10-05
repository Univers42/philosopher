/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memory.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 20:47:07 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/06 00:16:22 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char		*d;
	const unsigned char	*s;

	if (!dst || !src)
		return (dst);
	d = (unsigned char *)dst;
	s = (const unsigned char *)src;
	while (n--)
		*d++ = *s++;
	return (dst);
}

void	ft_swap(void *a, void *b, size_t n)
{
	unsigned char	*pa;
	unsigned char	*pb;
	unsigned char	tmp;

	if (!a || !b || a == b)
		return ;
	pa = (unsigned char *)a;
	pb = (unsigned char *)b;
	while (n--)
	{
		tmp = *pa;
		*pa++ = *pb;
		*pb++ = tmp;
	}
}