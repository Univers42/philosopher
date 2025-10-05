NAME	=	philo
CC		= 	cc
CFLAGS	=	-Wall -Wextra -Werror -pthread -Iinc

OBJDIR	:=	obj

SRCS = \
		src/utils/ft_ctype.c \
		src/utils/ft_stdio.c \
		src/utils/ft_stdlib.c \
		src/utils/ft_string.c \
		src/utils/ft_time.c \
		src/utils/ft_stdlib_stub1.c\
		src/utils/ft_stdlib_stub2.c\
		src/utils/ft_stdlib_stub3.c\
		src/utils/ft_stdlib_stub4.c\
		src/utils/helpers_conv.c \
		src/utils/states_conv.c \
		src/main.c

OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
