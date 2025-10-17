NAME			:=	philo
CC				:=	cc
CFLAGS			:=	-Wall -Wextra -Werror -MMD -MP
SRC_DIR			:= 	src
SRC				:=	actions.c	\
					clean.c		\
					init.c		\
					logs.c		\
					main.c		\
					parse.c		\
					routine.c	\
					sim_state.c	\
					utils.c
OBJS			:=	$(SRCS:.c=.o)
DEPS			:=	$(SRCS:.c=.d)
NAME_LIB		:=	minilibft.a
MINILIBFT_DIR	:=	$(NAME_LIB)/%.a
RM				:=	rm -f

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJ) -L$(MINILIBFT_DIR) -l:$(NAME_LIB) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run_tests: $(NAME)

build:
	$(MAKE) -C $(MINILIBFT_DIR) all

clean:
	$(RM) $(OBJS)

fclean:
	$(RM) $(NAME)
	$(MAKE) -C $(MINILIBFT_DIR) fclean

re: fclean all

.PHONY: all clean run_tests fclean build