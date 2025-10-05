/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_stdio.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 14:09:47 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/05 18:52:22 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

size_t	ft_strlen(const char *s);

void	ft_putchar_fd(const char s, int fd)
{
	write(fd, &s, 1);
}

void	ft_putstr_fd(const char *s, int fd)
{
	if (!(s && fd))
		return ;
	write(fd, s, ft_strlen(s));
}

void	ft_putnbr_fd(size_t nb, int fd)
{
	char	buf[32];
	int		i;
	int		start;

	i = 31;
	buf[i] = '\0';
	if (nb == 0)
	{
		ft_putchar_fd('0', fd);
		return ;
	}
	while (nb > 0)
	{
		buf[--i] = (nb % 10) + '0';
		nb /= 10;
	}
	start = i;
	ft_putstr_fd(&buf[start], fd);
}

//__attribute__((weak))
//int	main(int argc, char **argv)
//{
//	(void)argc;
//	ft_putchar_fd(argv[1][0], 1);
//	ft_putchar_fd('\n', 1);
//	ft_putstr_fd(argv[1], 1);
//	ft_putchar_fd('\n', 1);
//	ft_putnbr_fd(atoi(argv[2]), 1);
//	ft_putchar_fd('\n', 1);
//	return (0);
//}