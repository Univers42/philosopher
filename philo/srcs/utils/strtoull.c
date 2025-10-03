#include "philo.h"

// Type identifiers for generic conversion
typedef enum e_int_type {
	TYPE_UCHAR,
	TYPE_CHAR,
	TYPE_USHORT,
	TYPE_SHORT,
	TYPE_UINT,
	TYPE_INT,
	TYPE_ULONG,
	TYPE_LONG,
	TYPE_ULLONG,
	TYPE_LLONG,
	TYPE_UINT8,
	TYPE_INT8,
	TYPE_UINT16,
	TYPE_INT16,
	TYPE_UINT32,
	TYPE_INT32,
	TYPE_UINT64,
	TYPE_INT64
} t_int_type;

// Structure to hold conversion limits and properties
typedef struct s_type_info {
	uint64_t	max_val;
	int64_t		min_val;
	bool		is_signed;
} t_type_info;

// Conversion state machine states
typedef enum e_conv_state {
	STATE_INIT,
	STATE_WHITESPACE,
	STATE_SIGN,
	STATE_BASE_PREFIX,
	STATE_DIGITS,
	STATE_OVERFLOW,
	STATE_DONE,
	STATE_ERROR
} t_conv_state;

// Conversion context (state machine data)
typedef struct s_conv_ctx {
	const char		*ptr;
	const char		*start;
	char			**endptr;
	int				base;
	t_int_type		type;
	t_type_info		type_info;
	uint64_t		uval;
	bool			negative;
	bool			any_digit;
	t_conv_state	state;
} t_conv_ctx;

// Lookup table for type information (O(1) access)
static const t_type_info g_type_info_table[] = {
	[TYPE_UCHAR]   = {UCHAR_MAX, 0, false},
	[TYPE_CHAR]    = {CHAR_MAX, CHAR_MIN, true},
	[TYPE_USHORT]  = {USHRT_MAX, 0, false},
	[TYPE_SHORT]   = {SHRT_MAX, SHRT_MIN, true},
	[TYPE_UINT]    = {UINT_MAX, 0, false},
	[TYPE_INT]     = {INT_MAX, INT_MIN, true},
	[TYPE_ULONG]   = {ULONG_MAX, 0, false},
	[TYPE_LONG]    = {LONG_MAX, LONG_MIN, true},
	[TYPE_ULLONG]  = {ULLONG_MAX, 0, false},
	[TYPE_LLONG]   = {LLONG_MAX, LLONG_MIN, true},
	[TYPE_UINT8]   = {UINT8_MAX, 0, false},
	[TYPE_INT8]    = {INT8_MAX, INT8_MIN, true},
	[TYPE_UINT16]  = {UINT16_MAX, 0, false},
	[TYPE_INT16]   = {INT16_MAX, INT16_MIN, true},
	[TYPE_UINT32]  = {UINT_MAX, 0, false},
	[TYPE_INT32]   = {INT_MAX, INT_MIN, true},
	[TYPE_UINT64]  = {ULLONG_MAX, 0, false},
	[TYPE_INT64]   = {LLONG_MAX, LLONG_MIN, true}
};

// Get type information
static t_type_info	get_type_info(t_int_type type)
{
	if (type >= 0 && type < (int)(sizeof(g_type_info_table) / sizeof(g_type_info_table[0])))
		return (g_type_info_table[type]);
	return ((t_type_info){ULLONG_MAX, 0, false});
}

// Convert digit character to value
static int	char_to_digit(char c, int base)
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

// State: Skip whitespace
static void	state_whitespace(t_conv_ctx *ctx)
{
	while (isspace((unsigned char)*ctx->ptr))
		ctx->ptr++;
	ctx->state = STATE_SIGN;
}

// State: Parse sign
static void	state_sign(t_conv_ctx *ctx)
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
static void	state_base_prefix(t_conv_ctx *ctx)
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
		if (*ctx->ptr == '0' && (*(ctx->ptr + 1) == 'x' || *(ctx->ptr + 1) == 'X'))
			ctx->ptr += 2;
	}
	ctx->state = STATE_DIGITS;
}

// State: Parse digits with overflow detection
static void	state_digits(t_conv_ctx *ctx)
{
	int	digit;

	while (*ctx->ptr)
	{
		digit = char_to_digit(*ctx->ptr, ctx->base);
		if (digit == -1)
			break;
		if (ctx->uval > (UINT64_MAX - digit) / ctx->base)
		{
			ctx->state = STATE_OVERFLOW;
			return;
		}
		ctx->uval = ctx->uval * ctx->base + digit;
		ctx->ptr++;
		ctx->any_digit = true;
	}
	ctx->state = STATE_DONE;
}

// State: Handle overflow
static void	state_overflow(t_conv_ctx *ctx)
{
	errno = ERANGE;
	ctx->uval = UINT64_MAX;
	while (char_to_digit(*ctx->ptr, ctx->base) != -1)
		ctx->ptr++;
	ctx->any_digit = true;
	ctx->state = STATE_DONE;
}

// Initialize conversion context
static void	init_conv_ctx(t_conv_ctx *ctx, const char *nptr, char **endptr, 
							int base, t_int_type type)
{
	ctx->ptr = nptr;
	ctx->start = nptr;
	ctx->endptr = endptr;
	ctx->base = base;
	ctx->type = type;
	ctx->type_info = get_type_info(type);
	ctx->uval = 0;
	ctx->negative = false;
	ctx->any_digit = false;
	ctx->state = STATE_INIT;
}

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
	// Finalize and return result
	return (finalize_conversion(&ctx));
}

// Legacy compatibility wrappers
uint64_t	ft_strtoull(const char *nptr, char **endptr, int base)
{
	return ((uint64_t)ft_strto(nptr, endptr, base, TYPE_UINT64));
}

int64_t	ft_strtoint(const char *nptr, char **endptr, int base, t_int_type type)
{
	return (ft_strto(nptr, endptr, base, type));
}

uint64_t	ft_strtou64(const char *nptr, char **endptr, int base)
{
	return ((uint64_t)ft_strto(nptr, endptr, base, TYPE_UINT64));
}

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

unsigned int	ft_strto_uint(const char *nptr, char **endptr, int base)
{
	return ((unsigned int)ft_strto(nptr, endptr, base, TYPE_UINT));
}

int	ft_strto_int(const char *nptr, char **endptr, int base)
{
	return ((int)ft_strto(nptr, endptr, base, TYPE_INT));
}

unsigned long	ft_strto_ulong(const char *nptr, char **endptr, int base)
{
	return ((unsigned long)ft_strto(nptr, endptr, base, TYPE_ULONG));
}

long	ft_strto_long(const char *nptr, char **endptr, int base)
{
	return ((long)ft_strto(nptr, endptr, base, TYPE_LONG));
}

uint8_t	ft_strto_u8(const char *nptr, char **endptr, int base)
{
	return ((uint8_t)ft_strto(nptr, endptr, base, TYPE_UINT8));
}

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

// Helper function to determine type from size and signedness
static t_int_type	get_type_from_size(size_t size, bool is_unsigned)
{
	if (size == sizeof(unsigned char))
		return (is_unsigned ? TYPE_UCHAR : TYPE_CHAR);
	else if (size == sizeof(unsigned short))
		return (is_unsigned ? TYPE_USHORT : TYPE_SHORT);
	else if (size == sizeof(unsigned int))
		return (is_unsigned ? TYPE_UINT : TYPE_INT);
	else if (size == sizeof(unsigned long))
		return (is_unsigned ? TYPE_ULONG : TYPE_LONG);
	else if (size == sizeof(unsigned long long))
		return (is_unsigned ? TYPE_ULLONG : TYPE_LLONG);
	return (TYPE_INT64);
}

// Wrapper for unsigned integers
uint64_t	ft_strto_unsigned(const char *nptr, char **endptr, int base, size_t size)
{
	t_int_type	type;

	type = get_type_from_size(size, true);
	return ((uint64_t)ft_strto(nptr, endptr, base, type));
}

// Wrapper for signed integers
int64_t	ft_strto_signed(const char *nptr, char **endptr, int base, size_t size)
{
	t_int_type	type;

	type = get_type_from_size(size, false);
	return (ft_strto(nptr, endptr, base, type));
}

// Test main
#ifdef TEST_MAIN
int	main(void)
{
	char	*endptr;
	int64_t	result;
	
	printf("=== Testing UNIFIED ft_strto() function ===\n\n");
	
	printf("=== TYPE_UINT64 (unsigned 64-bit) ===\n");
	printf("\"12345\" base 10: %" PRIu64 "\n", (uint64_t)ft_strto("12345", &endptr, 10, TYPE_UINT64));
	printf("\"0xFF\" base 0: %" PRIu64 "\n", (uint64_t)ft_strto("0xFF", &endptr, 0, TYPE_UINT64));
	printf("\"777\" base 8: %" PRIu64 "\n", (uint64_t)ft_strto("777", &endptr, 8, TYPE_UINT64));
	printf("\"1010\" base 2: %" PRIu64 "\n", (uint64_t)ft_strto("1010", &endptr, 2, TYPE_UINT64));
	printf("\"ZZZ\" base 36: %" PRIu64 "\n", (uint64_t)ft_strto("ZZZ", &endptr, 36, TYPE_UINT64));
	
	printf("\n=== TYPE_UCHAR (0-255) ===\n");
	result = ft_strto("255", &endptr, 10, TYPE_UCHAR);
	printf("\"255\": %" PRId64 " (errno: %d)\n", result, errno);
	errno = 0;
	result = ft_strto("256", &endptr, 10, TYPE_UCHAR);
	printf("\"256\": %" PRId64 " (errno: %d - ERANGE)\n", result, errno);
	errno = 0;
	
	printf("\n=== TYPE_CHAR (-128 to 127) ===\n");
	printf("\"127\": %" PRId64 "\n", ft_strto("127", &endptr, 10, TYPE_CHAR));
	printf("\"-128\": %" PRId64 "\n", ft_strto("-128", &endptr, 10, TYPE_CHAR));
	result = ft_strto("128", &endptr, 10, TYPE_CHAR);
	printf("\"128\": %" PRId64 " (errno: %d - ERANGE)\n", result, errno);
	errno = 0;
	
	printf("\n=== TYPE_INT (-2147483648 to 2147483647) ===\n");
	printf("\"-2147483648\": %" PRId64 "\n", ft_strto("-2147483648", &endptr, 10, TYPE_INT));
	printf("\"2147483647\": %" PRId64 "\n", ft_strto("2147483647", &endptr, 10, TYPE_INT));
	printf("\"0x7FFFFFFF\": %" PRId64 "\n", ft_strto("0x7FFFFFFF", &endptr, 0, TYPE_INT));
	
	printf("\n=== TYPE_INT64 ===\n");
	printf("\"-9223372036854775808\" (MIN): %" PRId64 "\n", 
		ft_strto("-9223372036854775808", &endptr, 10, TYPE_INT64));
	printf("\"9223372036854775807\" (MAX): %" PRId64 "\n", 
		ft_strto("9223372036854775807", &endptr, 10, TYPE_INT64));
	
	printf("\n=== Different bases ===\n");
	printf("\"1111\" base 2: %" PRId64 "\n", ft_strto("1111", &endptr, 2, TYPE_INT));
	printf("\"0777\" base 0 (octal): %" PRId64 "\n", ft_strto("0777", &endptr, 0, TYPE_INT));
	printf("\"0x1A2B\" base 0 (hex): %" PRId64 "\n", ft_strto("0x1A2B", &endptr, 0, TYPE_INT));
	
	printf("\n=== Error handling ===\n");
	result = ft_strto("   ", &endptr, 10, TYPE_INT);
	printf("\"   \" (whitespace): %" PRId64 ", endptr: '%s'\n", result, endptr);
	
	result = ft_strto("123abc456", &endptr, 10, TYPE_INT);
	printf("\"123abc456\": %" PRId64 ", stopped at: '%s'\n", result, endptr);
	
	result = ft_strto("-255", &endptr, 10, TYPE_UCHAR);
	printf("\"-255\" for unsigned: %" PRId64 " (errno: %d)\n", result, errno);
	
	printf("\n=== Testing all integer types ===\n");
	printf("TYPE_UINT8:  %" PRId64 "\n", ft_strto("255", NULL, 10, TYPE_UINT8));
	printf("TYPE_INT8:   %" PRId64 "\n", ft_strto("-128", NULL, 10, TYPE_INT8));
	printf("TYPE_UINT16: %" PRId64 "\n", ft_strto("65535", NULL, 10, TYPE_UINT16));
	printf("TYPE_INT16:  %" PRId64 "\n", ft_strto("-32768", NULL, 10, TYPE_INT16));
	printf("TYPE_UINT32: %" PRId64 "\n", ft_strto("4294967295", NULL, 10, TYPE_UINT32));
	printf("TYPE_INT32:  %" PRId64 "\n", ft_strto("-2147483648", NULL, 10, TYPE_INT32));
	
	return (0);
}
#endif