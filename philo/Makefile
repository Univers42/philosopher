NAME    = philo
CC      = cc
CFLAGS  = -Wall -Wextra -Werror -pthread -I.

UTILSDIR := utils/
CUR_DIR  := .
OBJDIR   := obj/

SRCS = $(addprefix $(UTILSDIR), strtoull.c monitor.c time_utils.c precise_sleep.c)
SRCS += $(addprefix $(CUR_DIR)/, main.c)

# turn "utils/foo.c" into "obj/foo.o"
OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(notdir $(SRCS)))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

# compile utils/*.c
$(OBJDIR)/%.o: $(UTILSDIR)%.c
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# compile files in current dir
$(OBJDIR)/%.o: $(CUR_DIR)/%.c
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
