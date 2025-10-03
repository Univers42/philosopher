#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

size_t	ft_strlen(const char *s)
{
	const char *tmp = s;

	while (*tmp++)
		;
	return (tmp - s - 1);
}

void	ft_putchar_fd(char c, int fd)
{
	write(fd, &c, 1);
}

void    ft_putstr_fd(const char *s, int fd)
{
	write(1, s, ft_strlen(s));
}

void	ft_putnbr_fd(long n, int fd)
{
	static char	buf[32];
	static int	i;
	int			local_start;

	local_start = 0;
	if (i == 0)
		local_start = 1;
	if (n == LONG_MIN)
	{
		write(fd, "-9223372036854775808", 20);
		i = 0;
		return ;
	}
	if (n < 0)
	{
		buf[i++] = '-';
		n = -n;
	}
	if (n >= 10)
		ft_putnbr_fd(n / 10, fd);
	buf[i++] = (n % 10) + '0';
	if (local_start)
	{
		write(fd, buf, i);
		i = 0;
	}
}

__attribute__((weak))
int main(void)
{
	ft_putnbr_fd(-121321352416511223, 1);
	ft_putchar_fd('\n', 1);
	return (0);
}