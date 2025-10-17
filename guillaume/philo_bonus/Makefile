# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: glacroix <PGCL>                            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/14 18:36:04 by glacroix          #+#    #+#              #
#    Updated: 2024/09/11 17:55:18 by glacroix         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#Colors
# **************************************************************************** #
ifeq ($(shell uname), Linux)
	RESET		= '\033[0m'
	RED			= '\033[0;31m'
	GREEN		= '\033[0;32m'
	YELLOW  	= '\033[1;33m'
	MAGENTA 	= '\033[0;35m' 
	CYAN  		= '\033[0;36m'
	WHITE  		= '\033[1;37m'
else
    RED = "\x1B[31m"
    GREEN = "\x1B[32m"
    YELLOW = "\x1B[33m"
    BLUE = "\x1B[34m"
    MAGENTA = "\x1B[35m"
    CAYN = "\x1B[36m"
    WHITE = "\x1B[37m"
endif
#Program name
# **************************************************************************** #
NAME		= philo_bonus

#SRC & OBJS Details
# **************************************************************************** #
SRCS		= srcs/main_bonus.c\
			  srcs/base_setup_bonus.c\
			  srcs/utils_bonus.c\
			  srcs/utils2_bonus.c\
			  srcs/init_bonus.c\
			  srcs/routine_bonus.c\

OBJS		= $(SRCS:%.c=objs/%.o)

#Execution
# **************************************************************************** #
CC			= gcc 
CFLAGS		= -Wall -Werror -Wextra -lpthread  #-fsanitize=address

#SRC Execution
# **************************************************************************** #
${NAME}: objs ${OBJS}
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo $(MAGENTA) "\n~~~~~~~~~ Philo Bonus compiled! ~~~~~~~~\n" $(RESET)

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
	@echo $(RED) "\n>>>>>>>>>>> deleting object files <<<<<<<<<\n" $(RESET)

re: fclean all

re_bonus: fclean bonus

.PHONY: all clean fclean re re_bonus bonus
