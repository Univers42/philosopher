/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_stdlib_stub3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 18:45:32 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/05 19:00:24 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "conv.h"

int64_t	ft_strto64(const char *nptr, char **endptr, int base)
{
	return (ft_strto(nptr, endptr, base, TYPE_INT64));
}

// Type-specific conversion functions (no sizeof ambiguity!)
unsigned char	ft_strto_uchar(const char *nptr, char **endptr, int base)
{
	return ((unsigned char)ft_strto(nptr, endptr, base, TYPE_UCHAR));
}

signed char	ft_strto_char(const char *nptr, char **endptr, int base)
{
	return ((signed char)ft_strto(nptr, endptr, base, TYPE_CHAR));
}

unsigned short	ft_strto_ushort(const char *nptr, char **endptr, int base)
{
	return ((unsigned short)ft_strto(nptr, endptr, base, TYPE_USHORT));
}

short	ft_strto_short(const char *nptr, char **endptr, int base)
{
	return ((short)ft_strto(nptr, endptr, base, TYPE_SHORT));
}
