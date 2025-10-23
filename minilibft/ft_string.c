/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_string.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 18:39:49 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 06:17:02 by dlesieur         ###   ########.fr       */
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

int	ft_strncmp(const char *msg, const char *s2, size_t n)
{
	if (!msg || !s2 || n == 0)
		return (0);
	while (n--)
	{
		if ((unsigned char)*msg != (unsigned char)*s2)
			return ((unsigned char)*msg - (unsigned char)*s2);
		if (*msg == '\0')
			return (0);
		msg++;
		s2++;
	}
	return (0);
}
