NAME := philo

CC := cc
CFLAGS := -Wall -Wextra -Werror
LDFLAGS := -pthread
INCLUDES := -I utils

# Enable with: make DEBUG=1
ifdef DEBUG
CFLAGS += -DDEBUG
endif

ifdef DEATH_CHECK
CFLAGS += -DDEATH_CHECK
endif

ifdef FORK_CHECK
CFLAGS += -DFORK_CHECK
endif

SRCS := \
	utils/philo.c \
	utils/helpers.c \
	utils/pthread_attr.c \
	main.c \
	routine.c \
	waiters.c\
	ctr_lock.c

OBJS := $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c utils/philosopher.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
