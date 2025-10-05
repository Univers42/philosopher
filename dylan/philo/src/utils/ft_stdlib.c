/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_stdlib.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 14:09:38 by dlesieur          #+#    #+#             */
/*   Updated: 2025/10/05 19:14:08 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/conv.h"

// Finalize conversion and apply type constraints
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
	{
		if (ctx->uval > (uint64_t)-(ctx->type_info.min_val))
		{
			errno = ERANGE;
			return (ctx->type_info.min_val);
		}
		result = -(int64_t)ctx->uval;
	}
	else
	{
		if (ctx->uval > ctx->type_info.max_val)
		{
			errno = ERANGE;
			return (ctx->type_info.max_val);
		}
		result = (int64_t)ctx->uval;
	}
	return (result);
}

// UNIFIED CONVERSION FUNCTION - Handles all integer types
int64_t	ft_strto(const char *nptr, char **endptr, int base, t_int_type type)
{
	t_conv_ctx	ctx;

	// Validate base
	if (base < 0 || base == 1 || base > 36)
	{
		errno = EINVAL;
		if (endptr)
			*endptr = (char *)nptr;
		return (0);
	}
	// Initialize state machine
	init_conv_ctx(&ctx, nptr, endptr, base, type);
	// Run state machine
	while (ctx.state != STATE_DONE && ctx.state != STATE_ERROR)
	{
		if (ctx.state == STATE_INIT)
			ctx.state = STATE_WHITESPACE;
		else if (ctx.state == STATE_WHITESPACE)
			state_whitespace(&ctx);
		else if (ctx.state == STATE_SIGN)
			state_sign(&ctx);
		else if (ctx.state == STATE_BASE_PREFIX)
			state_base_prefix(&ctx);
		else if (ctx.state == STATE_DIGITS)
			state_digits(&ctx);
		else if (ctx.state == STATE_OVERFLOW)
			state_overflow(&ctx);
	}
	return (finalize_conversion(&ctx));
}

//__attribute__((weak))
// int	main(void)
// {
// 	char	*endptr;
// 	int64_t	result;
	
// 	printf("=== Testing UNIFIED ft_strto() function ===\n\n");
	
// 	printf("=== TYPE_UINT64 (unsigned 64-bit) ===\n");
// 	printf("\"12345\" base 10: %" PRIu64 "\n", (uint64_t)ft_strto("12345", &endptr, 10, TYPE_UINT64));
// 	printf("\"0xFF\" base 0: %" PRIu64 "\n", (uint64_t)ft_strto("0xFF", &endptr, 0, TYPE_UINT64));
// 	printf("\"777\" base 8: %" PRIu64 "\n", (uint64_t)ft_strto("777", &endptr, 8, TYPE_UINT64));
// 	printf("\"1010\" base 2: %" PRIu64 "\n", (uint64_t)ft_strto("1010", &endptr, 2, TYPE_UINT64));
// 	printf("\"ZZZ\" base 36: %" PRIu64 "\n", (uint64_t)ft_strto("ZZZ", &endptr, 36, TYPE_UINT64));
	
// 	printf("\n=== TYPE_UCHAR (0-255) ===\n");
// 	result = ft_strto("255", &endptr, 10, TYPE_UCHAR);
// 	printf("\"255\": %" PRId64 " (errno: %d)\n", result, errno);
// 	errno = 0;
// 	result = ft_strto("256", &endptr, 10, TYPE_UCHAR);
// 	printf("\"256\": %" PRId64 " (errno: %d - ERANGE)\n", result, errno);
// 	errno = 0;
	
// 	printf("\n=== TYPE_CHAR (-128 to 127) ===\n");
// 	printf("\"127\": %" PRId64 "\n", ft_strto("127", &endptr, 10, TYPE_CHAR));
// 	printf("\"-128\": %" PRId64 "\n", ft_strto("-128", &endptr, 10, TYPE_CHAR));
// 	result = ft_strto("128", &endptr, 10, TYPE_CHAR);
// 	printf("\"128\": %" PRId64 " (errno: %d - ERANGE)\n", result, errno);
// 	errno = 0;
	
// 	printf("\n=== TYPE_INT (-2147483648 to 2147483647) ===\n");
// 	printf("\"-2147483648\": %" PRId64 "\n", ft_strto("-2147483648", &endptr, 10, TYPE_INT));
// 	printf("\"2147483647\": %" PRId64 "\n", ft_strto("2147483647", &endptr, 10, TYPE_INT));
// 	printf("\"0x7FFFFFFF\": %" PRId64 "\n", ft_strto("0x7FFFFFFF", &endptr, 0, TYPE_INT));
	
// 	printf("\n=== TYPE_INT64 ===\n");
// 	printf("\"-9223372036854775808\" (MIN): %" PRId64 "\n", 
// 		ft_strto("-9223372036854775808", &endptr, 10, TYPE_INT64));
// 	printf("\"9223372036854775807\" (MAX): %" PRId64 "\n", 
// 		ft_strto("9223372036854775807", &endptr, 10, TYPE_INT64));
	
// 	printf("\n=== Different bases ===\n");
// 	printf("\"1111\" base 2: %" PRId64 "\n", ft_strto("1111", &endptr, 2, TYPE_INT));
// 	printf("\"0777\" base 0 (octal): %" PRId64 "\n", ft_strto("0777", &endptr, 0, TYPE_INT));
// 	printf("\"0x1A2B\" base 0 (hex): %" PRId64 "\n", ft_strto("0x1A2B", &endptr, 0, TYPE_INT));
	
// 	printf("\n=== Error handling ===\n");
// 	result = ft_strto("   ", &endptr, 10, TYPE_INT);
// 	printf("\"   \" (whitespace): %" PRId64 ", endptr: '%s'\n", result, endptr);
	
// 	result = ft_strto("123abc456", &endptr, 10, TYPE_INT);
// 	printf("\"123abc456\": %" PRId64 ", stopped at: '%s'\n", result, endptr);
	
// 	result = ft_strto("-255", &endptr, 10, TYPE_UCHAR);
// 	printf("\"-255\" for unsigned: %" PRId64 " (errno: %d)\n", result, errno);
	
// 	printf("\n=== Testing all integer types ===\n");
// 	printf("TYPE_UINT8:  %" PRId64 "\n", ft_strto("255", NULL, 10, TYPE_UINT8));
// 	printf("TYPE_INT8:   %" PRId64 "\n", ft_strto("-128", NULL, 10, TYPE_INT8));
// 	printf("TYPE_UINT16: %" PRId64 "\n", ft_strto("65535", NULL, 10, TYPE_UINT16));
// 	printf("TYPE_INT16:  %" PRId64 "\n", ft_strto("-32768", NULL, 10, TYPE_INT16));
// 	printf("TYPE_UINT32: %" PRId64 "\n", ft_strto("4294967295", NULL, 10, TYPE_UINT32));
// 	printf("TYPE_INT32:  %" PRId64 "\n", ft_strto("-2147483648", NULL, 10, TYPE_INT32));
	
// 	return (0);
// }