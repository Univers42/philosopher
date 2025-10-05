#include <unistd.h>
#include <limits.h>
#include "philo.h"

size_t	ft_strlen(const char *s)
{
	const char *p = s;

	while (*p)
		p++;
	return (size_t)(p - s);
}

void	ft_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
}

void    ft_putstr_fd(const char *s, int fd)
{
	if (!s)
		return;
	write(fd, s, ft_strlen(s));
}

void	ft_putnbr_fd(long n, int fd)
{
	char	buf[32];
	int		i = 0;
	long	tmp = n;

	if (n == LONG_MIN)
		return (write(fd, "-9223372036854775808", 20), (void)0);
	if (n < 0)
	{
		ft_putchar_fd('-', fd);
		tmp = -n;
	}
	if (tmp == 0)
		return (ft_putchar_fd('0', fd), (void)0);
	while (tmp)
	{
		buf[i++] = (char)('0' + (tmp % 10));
		tmp /= 10;
	}
	while (i--)
		ft_putchar_fd(buf[i], fd);
}