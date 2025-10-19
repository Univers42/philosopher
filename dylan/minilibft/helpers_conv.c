/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers_conv.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 19:06:34 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 15:07:35 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "conv.h"

// Get type information
const t_type_info	*get_type_info(t_int_type type)
{
	const t_type_info	*g_type_info_table = type_info_table();

	if (type >= 0 && type < TYPE_INFO_COUNT)
		return (&g_type_info_table[type]);
	return (NULL);
}

// Convert digit character to value
int	char_to_digit(char c, int base)
{
	int	digit;

	if (isdigit((unsigned char)c))
		digit = c - '0';
	else if (isalpha((unsigned char)c))
		digit = tolower((unsigned char)c) - 'a' + 10;
	else
		return (-1);
	if (digit >= base)
		return (-1);
	return (digit);
}

// Initialize conversion context
void	init_conv_ctx(t_conv_ctx *ctx, const char *nptr,
					int base, t_int_type type)
{
	ctx->ptr = nptr;
	ctx->start = nptr;
	ctx->endptr = NULL;
	ctx->base = base;
	ctx->type = type;
	ctx->type_info = *get_type_info(type);
	ctx->uval = 0;
	ctx->negative = false;
	ctx->any_digit = false;
	ctx->state = STATE_INIT;
}
