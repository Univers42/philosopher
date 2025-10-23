# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/01/19 12:00:00 by dlesieur          #+#    #+#              #
#    Updated: 2025/10/19 13:47:03 by dlesieur         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ============================================================================ #
#                                   COLORS                                     #
# ============================================================================ #

RED			= \033[0;31m
GREEN		= \033[0;32m
YELLOW		= \033[0;33m
BLUE		= \033[0;34m
MAGENTA		= \033[0;35m
CYAN		= \033[0;36m
WHITE		= \033[0;37m
BOLD		= \033[1m
RESET		= \033[0m

# ============================================================================ #
#                                CONFIGURATION                                 #
# ============================================================================ #

NAME		:= philo
CC			:= cc
CFLAGS		:= -Wall -Wextra -Werror -MMD -MP -I../minilibft
LDFLAGS		:= -pthread -O3 -std=c99
OBJ_DIR		:= obj
BIN_DIR		:= bin

# Optional: Enable with 'make SANITIZE=1'
ifdef SANITIZE
	CFLAGS += -fsanitize=thread -g
	LDFLAGS += -fsanitize=thread
endif

# Debug mode: Enable with 'make DEBUG=1'
ifdef DEBUG
	CFLAGS += -DDEBUG_MODE=1 -g
else
	CFLAGS += -DDEBUG_MODE=0
endif

# Computed build mode label
MODE		:= $(if $(DEBUG),DEBUG,RELEASE)

# ============================================================================ #
#                                   SOURCES                                    #
# ============================================================================ #

SRCS		:= actions.c		\
			   helpers.c		\
			   init.c			\
			   logs.c			\
			   main.c			\
			   parse.c			\
			   routine.c		\
			   sim_state.c		\
			   log_helpers1.c	\
			   log_helpers2.c	\
			   log_helpers3.c

OBJS		:= $(SRCS:%.c=$(OBJ_DIR)/%.o)
DEPS		:= $(OBJS:.o=.d)

MINILIB_DIR	:= ../minilibft
MINILIB		:= $(MINILIB_DIR)/libminft.a
RM			:= rm -f

# Extra controls
VERBOSE		?= 0
ifeq ($(VERBOSE),1)
MINILIB_MAKEFLAGS :=
else
MINILIB_MAKEFLAGS := -s
endif
BUILD_START	:= .build_start

TOTAL_SRCS	:= $(words $(SRCS))
CURRENT		:= 0

# ============================================================================ #
#                                   RULES                                      #
# ============================================================================ #

all: $(BIN_DIR)/$(NAME)

$(BIN_DIR)/$(NAME): create_dir $(OBJS) $(MINILIB)
	@date +%s > $(BUILD_START)
	@echo "$(CYAN)> Building:$(RESET) $(WHITE)$(BIN_DIR)/$(NAME)$(RESET)"
	@echo "$(CYAN)> Mode:    $(RESET) $(MAGENTA)$(MODE)$(RESET)  $(CYAN)| Compiler:$(RESET) $(YELLOW)$(CC)$(RESET)"
	@echo "$(CYAN)> Flags:   $(RESET) $(YELLOW)$(CFLAGS)$(RESET)"
	@$(CC) $(CFLAGS) $(OBJS) $(MINILIB) $(LDFLAGS) -o $@
	@bin_size=$$(test -f $(BIN_DIR)/$(NAME) && wc -c < $(BIN_DIR)/$(NAME) || echo 0); \
	 human=$$(awk 'BEGIN{n=$(shell test -f $(BIN_DIR)/$(NAME) && wc -c < $(BIN_DIR)/$(NAME) || echo 0); s="BKMGT"; while(n>1024){n/=1024;i++} printf "%.1f%s", n, substr(s,i+1,1)}'); \
	 start=$$(cat $(BUILD_START) 2>/dev/null || echo 0); end=$$(date +%s); dur=$$((end-start)); \
	 echo "$(GREEN)> Done.$(RESET) Size: $${bin_size} ($${human}) | Duration: $${dur}s";

$(OBJ_DIR)/%.o: %.c | create_dir
	@$(eval CURRENT=$(shell echo $$(($(CURRENT)+1))))
	@printf "$(CYAN)[%2d/$(TOTAL_SRCS)]$(RESET) Compiling $(YELLOW)%-20s$(RESET) " $(CURRENT) $<
	@$(CC) $(CFLAGS) -c $< -o $@ 2>&1 | tee /tmp/compile_err_$$$$ || \
		(cat /tmp/compile_err_$$$$; rm -f /tmp/compile_err_$$$$; exit 1)
	@rm -f /tmp/compile_err_$$$$
	@echo "$(GREEN)✓$(RESET)"

create_dir:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

run: $(BIN_DIR)/$(NAME)
	@if [ ! -x test.sh ]; then chmod +x test.sh; fi
	@echo "$(CYAN)> Running tests...$(RESET)"
	@./test.sh

clean:
	@$(RM) $(OBJS) $(DEPS)
	@rm -rf $(OBJ_DIR)
	@echo "$(YELLOW)> Cleaned objects.$(RESET)"

fclean: clean
	@$(RM) $(BIN_DIR)/$(NAME)
	@$(MAKE) $(MINILIB_MAKEFLAGS) -C $(MINILIB_DIR) fclean --no-print-directory
	@echo "$(YELLOW)> Full clean complete.$(RESET)"

re:
	@$(MAKE) fclean --no-print-directory
	@$(MAKE) all --no-print-directory

# ============================================================================ #
#                                   PHONY                                      #
# ============================================================================ #

-include $(DEPS)

.PHONY: all clean fclean re run
.SILENT:

$(MINILIB):
	@echo "$(YELLOW)⚙  Building minilibft...$(RESET)"
	@$(MAKE) $(MINILIB_MAKEFLAGS) -C $(MINILIB_DIR) all --no-print-directory
	@if [ ! -f $(MINILIB) ]; then cp $(MINILIB_DIR)/libminft.a $(MINILIB); fi
	@echo "$(GREEN)✓  Minilibft ready$(RESET)"
	@echo ""