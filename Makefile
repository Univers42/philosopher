#CONFIG VARIABLES

cc = cc
CFLAGS = -Wall -Wextra -Werror
RM = rm -f
NAME = philosopher
SRCS = GNU_example.c
OBJECTS = $(SRCS:.c=.o)
#FILES
#TARGETS

all: $NAME

%.o: %.c
	$(cc) $(CFLAGS) -c $< -o $@

$NAME: main.o
	$(cc) $(CFLAGS) -o $(NAME) $(OBJECTS) -lpthread

clean: 
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(NAME)

re: fclean all
.PHONY: all clean fclean re
