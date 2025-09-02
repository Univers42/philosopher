#CONFIG VARIABLES

cc = cc
CFLAGS = -Wall -Wextra -Werror
RM = rm -f
NAME = philosopher
SRCS = main.c
OBJECTS = $(SRCS:.c=.o)
#FILES
#TARGETS
all: $NAME

$NAME: main.o
	$(cc) $(CFLAGS) -o $(NAME) main.o -lpthread

clean: 
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(NAME)

re: fclean all
.PHONY: all clean fclean re
