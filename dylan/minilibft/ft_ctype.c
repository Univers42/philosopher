/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ctype.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 14:09:52 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/14 18:51:13 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

int	ft_isspace(int c)
{
	return (c == ' ' || (c >= 8 && c <= 13));
}

int	ft_isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

int	ft_isalpha(int c)
{
	return ((c >= 'a' && c <= 'z')
		|| (c >= 'A' && c <= 'Z'));
}

int	ft_is_number(const char *str)
{
	const char	*cur = str;

	while (*cur)
	{
		if (!ft_isdigit(*cur))
			return (false);
		cur++;
	}
	return (true);
}

// __attribute__((weak))
// int	main(int argc, char **argv)
// {
// 	(void)argc;
// 	(void)argv;
// 	return (0);
// }