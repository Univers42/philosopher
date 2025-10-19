/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_stdlib.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 14:09:38 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/19 15:11:07 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "conv.h"

// Finalize conversion and apply type constraints
static int64_t	finalize_negative_conversion(t_conv_ctx *ctx)
{
	int64_t	result;

	if (ctx->uval > (uint64_t)-(ctx->type_info.min_val))
	{
		errno = ERANGE;
		return (ctx->type_info.min_val);
	}
	result = -(int64_t)ctx->uval;
	return (result);
}

static int64_t	finalize_positive_conversion(t_conv_ctx *ctx)
{
	int64_t	result;

	if (ctx->uval > ctx->type_info.max_val)
	{
		errno = ERANGE;
		return (ctx->type_info.max_val);
	}
	result = (int64_t)ctx->uval;
	return (result);
}

static int64_t	finalize_conversion(t_conv_ctx *ctx)
{
	int64_t	result;

	if (!ctx->any_digit)
	{
		if (ctx->endptr)
			*ctx->endptr = (char *)ctx->start;
		return (0);
	}
	if (ctx->endptr)
		*ctx->endptr = (char *)ctx->ptr;
	if (ctx->negative && !ctx->type_info.is_signed)
	{
		errno = ERANGE;
		return (0);
	}
	if (ctx->negative)
		result = finalize_negative_conversion(ctx);
	else
		result = finalize_positive_conversion(ctx);
	return (result);
}

// UNIFIED CONVERSION FUNCTION - Handles all integer types
int64_t	ft_strto(const char *nptr, char **endptr, int base, t_int_type type)
{
	t_conv_ctx			ctx;
	const t_fn_state	*table = conv_state_table();
	t_fn_state			fn;

	if (base < 0 || base == 1 || base > 36)
	{
		errno = EINVAL;
		if (endptr)
			*endptr = (char *)nptr;
		return (0);
	}
	ctx.endptr = endptr;
	init_conv_ctx(&ctx, nptr, base, type);
	if (ctx.state == STATE_INIT)
		ctx.state = STATE_WHITESPACE;
	while (ctx.state != STATE_DONE && ctx.state != STATE_ERROR)
	{
		fn = table[ctx.state];
		if (!fn)
			break ;
		fn(&ctx);
	}
	return (finalize_conversion(&ctx));
}

void	*ft_calloc(size_t nmemb, size_t size)
{
	size_t	total;
	void	*ptr;

	if (size != 0 && nmemb > (SIZE_MAX / size))
		return (NULL);
	total = nmemb * size;
	ptr = malloc(total);
	if (!ptr)
		return (NULL);
	memset(ptr, 0, total);
	return (ptr);
}
