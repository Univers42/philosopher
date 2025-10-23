CC		:=	cc
CFLAGS	:=	-Wall -Wextra -Werror -Wshadow
NAME	:=	libminft.a
SRCS	:=	ft_ctype.c ft_memory.c ft_stdio.c ft_stdlib_stub1.c ft_stdlib_stub2.c \
			ft_stdlib_stub3.c ft_stdlib_stub4.c ft_stdlib.c ft_string.c ft_time.c \
			ft_time_helper1.c ft_time_helper2.c ft_time_helper3.c ft_time_helper4.c \
			helpers_conv.c states_conv.c
OBJ_DIR := obj
OBJS	:= $(SRCS:%.c=$(OBJ_DIR)/%.o)
RM		:=	rm -rf
AR 		:=	ar rcs

all: $(NAME)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJS)
	$(AR) $@ $(OBJS)

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re