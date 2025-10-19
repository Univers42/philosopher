/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_math.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 14:51:57 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 06:22:09 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdbool.h>

bool	within_range(unsigned short min, unsigned short max, unsigned short v)
{
	return ((v - min) <= (max - min));
}

double	ft_sqrt(double x)
{
	double	guess;
	double	prev;
	int		i;

	if (x <= 0.0)
		return (0.0);
	guess = x;
	prev = 0.0;
	i = -1;
	while (++i < 12 && guess != prev)
	{
		prev = guess;
		guess = 0.5 * (guess + x / guess);
	}
	return (guess);
}
