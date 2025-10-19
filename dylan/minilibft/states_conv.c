/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   states_conv.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 19:03:28 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 06:18:45 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "conv.h"

// State: Skip whitespace
void	state_whitespace(t_conv_ctx *ctx)
{
	while (isspace((unsigned char)*ctx->ptr))
		ctx->ptr++;
	ctx->state = STATE_SIGN;
}

// State: Parse sign
void	state_sign(t_conv_ctx *ctx)
{
	if (*ctx->ptr == '-')
	{
		ctx->negative = true;
		ctx->ptr++;
	}
	else if (*ctx->ptr == '+')
		ctx->ptr++;
	ctx->state = STATE_BASE_PREFIX;
}

// State: Parse base prefix
void	state_base_prefix(t_conv_ctx *ctx)
{
	if (ctx->base == 0)
	{
		if (*ctx->ptr == '0')
		{
			if (*(ctx->ptr + 1) == 'x' || *(ctx->ptr + 1) == 'X')
			{
				ctx->base = 16;
				ctx->ptr += 2;
			}
			else
			{
				ctx->base = 8;
				ctx->ptr += 1;
			}
		}
		else
			ctx->base = 10;
	}
	else if (ctx->base == 16)
	{
		if (*ctx->ptr == '0' && (*(ctx->ptr + 1) == 'x'
				|| *(ctx->ptr + 1) == 'X'))
			ctx->ptr += 2;
	}
	ctx->state = STATE_DIGITS;
}

// State: Parse digits with overflow detection
void	state_digits(t_conv_ctx *ctx)
{
	int	digit;

	while (*ctx->ptr)
	{
		digit = char_to_digit(*ctx->ptr, ctx->base);
		if (digit == -1)
			break ;
		if (ctx->uval > (UINT64_MAX - digit) / ctx->base)
		{
			ctx->state = STATE_OVERFLOW;
			return ;
		}
		ctx->uval = ctx->uval * ctx->base + digit;
		ctx->ptr++;
		ctx->any_digit = true;
	}
	ctx->state = STATE_DONE;
}

// State: Handle overflow
void	state_overflow(t_conv_ctx *ctx)
{
	errno = ERANGE;
	ctx->uval = UINT64_MAX;
	while (char_to_digit(*ctx->ptr, ctx->base) != -1)
		ctx->ptr++;
	ctx->any_digit = true;
	ctx->state = STATE_DONE;
}
