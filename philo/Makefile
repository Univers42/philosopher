NAME    = philo
CC      = cc
CFLAGS  = -Wall -Wextra -Werror -pthread -Iinc

OBJDIR  := obj

SRCS = \
	srcs/main.c \
	srcs/helpers/init.c \
	srcs/helpers/safe_thread_methods.c \
	srcs/helpers/safe_thread_check.c \
	srcs/routines.c \
	srcs/utils/time_utils.c \
	srcs/utils/util_str.c \
	srcs/utils/strtoull.c \
	srcs/utils/ft_exit.c

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
