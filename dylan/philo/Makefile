# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/10/15 12:49:36 by dlesieur          #+#    #+#              #
#    Updated: 2025/10/15 12:55:54 by dlesieur         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			=	philo
CC				= 	cc
CFLAGS			=	-Wall -Wextra -Werror -pthread -I. -I../minilibft -MMD
OBJDIR			:=	obj
DIR_MINILIBFT	:= ../minilibft
SRCS = \
		init.c \
		actions.c \
		checking.c \
		helper1.c \
		debug.c \
		routines.c \
		main.c
OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS) | build
	@$(CC) $(CFLAGS) $(OBJS) -L$(DIR_MINILIBFT) -lminft -o $(NAME)

build:
	$(MAKE) -C $(DIR_MINILIBFT)

MAKEFLAGS = --no-print-directory

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR)

fclean: clean
	@rm -f $(NAME)
	$(MAKE) -C $(DIR_MINILIBFT) fclean

re: fclean all

.PHONY: all clean fclean re build