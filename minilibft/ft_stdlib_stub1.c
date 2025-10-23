/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_stdlib_stub1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 18:44:30 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/05 19:00:18 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "conv.h"

int8_t	ft_strto_i8(const char *nptr, char **endptr, int base)
{
	return ((int8_t)ft_strto(nptr, endptr, base, TYPE_INT8));
}

uint16_t	ft_strto_u16(const char *nptr, char **endptr, int base)
{
	return ((uint16_t)ft_strto(nptr, endptr, base, TYPE_UINT16));
}

int16_t	ft_strto_i16(const char *nptr, char **endptr, int base)
{
	return ((int16_t)ft_strto(nptr, endptr, base, TYPE_INT16));
}

uint32_t	ft_strto_u32(const char *nptr, char **endptr, int base)
{
	return ((uint32_t)ft_strto(nptr, endptr, base, TYPE_UINT32));
}

int32_t	ft_strto_i32(const char *nptr, char **endptr, int base)
{
	return ((int32_t)ft_strto(nptr, endptr, base, TYPE_INT32));
}
