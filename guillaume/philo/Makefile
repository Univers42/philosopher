# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: glacroix <glacroix@student.42madrid.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/14 18:36:04 by glacroix          #+#    #+#              #
#    Updated: 2023/09/26 15:52:34 by glacroix         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#Colors
# **************************************************************************** #
RESET  		= "\x1B[0m"
RED  		= "\x1B[31m"
GREEN  		= "\x1B[32m"
YELLOW  	= "\x1B[33m"
BLUE 		= "\x1B[34m"
MAGENTA 	= "\x1B[35m"
CAYN  		= "\x1B[36m"
WHITE  		= "\x1B[37m"

#Program name
# **************************************************************************** #
NAME		= philo

#SRC & OBJS Details
# **************************************************************************** #
SRCS		= srcs/main.c\
				srcs/init.c\
				srcs/checker.c\
				srcs/utils.c\
				srcs/utils2.c\
				srcs/routine.c\

OBJS		= $(SRCS:%.c=objs/%.o)

#Execution
# **************************************************************************** #
CC			= gcc 
CFLAGS		= -Wall -Werror -Wextra -pthread #-g -fsanitize=address

#SRC Execution
# **************************************************************************** #
${NAME}: objs ${OBJS}
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo $(GREEN) "\n         Philo Compiled!       \n" $(RESET)

#Makefile Cmds
# **************************************************************************** #
objs:
	@mkdir -p objs/srcs

objs/%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@ 

all: $(NAME)

clean:
	@rm -rf objs

fclean: clean
	@rm -f $(NAME)
	@echo $(RED) "\n >>>>>>>> Deleted all *.o ! <<<<<<<< \n" $(RESET)

re: fclean all

re_bonus: fclean bonus

.PHONY: all clean fclean re re_bonus bonus
