# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mvassall <mvassall@student.42madrid.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/12/11 11:05:02 by user1             #+#    #+#              #
#    Updated: 2025/07/11 15:54:36 by mvassall         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC := cc
CFLAGS := -Wall -Werror -Wextra -pthread -g
#CFLAGS := -Wall -Werror -Wextra -g3
RM := /bin/rm -f
SRC_DIRS := ./src
BUILD_DIR := ./build

SRCS := src/main.c \
	src/philo_utils_1.c \
	src/philo_utils_2.c \
	src/philo_utils_3.c \
	src/philo_utils_4.c \
	src/philosopher_1.c \
	src/philosopher_2.c

OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

NAME = philo

.PHONY: all clean fclean re

all: $(NAME)

show:
	echo $(OBJS)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS)

$(OBJS): include/philo.h

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I./include -o $@ -c $<

clean:
	$(RM) -r $(BUILD_DIR)

fclean: clean
	$(RM) $(NAME) 

re: fclean all
