NAME			:= philo
CC			:= cc
CFLAGS		:= -Wall -Wextra -Werror -MMD -MP -I../minilibft
LDFLAGS		:= -pthread -O3 -std=c99

# Optional: Enable with 'make SANITIZE=1'
ifdef SANITIZE
	CFLAGS += -fsanitize=thread -g
	LDFLAGS += -fsanitize=thread
endif

SRCS		:= actions.c \
			   helpers.c \
			   init.c \
			   logs.c \
			   main.c \
			   parse.c \
			   routine.c \
			   sim_state.c

OBJS		:= $(SRCS:.c=.o)
DEPS		:= $(SRCS:.c=.d)

MINILIB_DIR	:= ../minilibft
MINILIB		:= $(MINILIB_DIR)/libminft.a
RM			:= rm -f

all: $(NAME)

$(MINILIB):
	$(MAKE) -C $(MINILIB_DIR) all

$(NAME): $(OBJS) $(MINILIB)
	$(CC) $(CFLAGS) $(OBJS) $(MINILIB) $(LDFLAGS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(NAME)
	if [ ! -x test.sh ];then 	\
		chmod +x test.sh;		\
	fi;							\
	./test.sh

clean:
	$(RM) $(OBJS) $(DEPS)

fclean: clean
	$(RM) $(NAME)
	$(MAKE) -C $(MINILIB_DIR) fclean

re: fclean all

-include $(DEPS)

.PHONY: all clean run_tests fclean re