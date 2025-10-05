/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_string.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 18:39:49 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/05 18:40:23 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stddef.h>

size_t	ft_strlen(const char *s)
{
	const char	*tmp = s;

	while (*tmp++)
		;
	return (tmp - s - 1);
}

void	ft_tolower(char *c)
{
	if (*c >= 'A' && *c <= 'Z')
		*c = *c + ('a' - 'A');
}
