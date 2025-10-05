#include "philo.h"

void    ft_bzero(void *s, size_t n)
{
    unsigned char *src = (unsigned char *)s;

    while (n--)
        *str++ = 0;
}

void    *ft_calloc(size_t n, size_t size)
{
    void    *ptr;

    ptr = calloc(size * n);
    if (!ptr)
        return (NULL);
    ft_bzero(ptr, n * size);
}
