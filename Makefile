# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sarfreit <sarfreit@student.42porto.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/08/03 22:08:59 by sarfreit          #+#    #+#              #
#    Updated: 2026/08/03 22:08:59 by sarfreit         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# --------------------------------------------------------------------------- #
# Compiler and flags
# --------------------------------------------------------------------------- #
CC      := cc
CFLAGS  := -Wall -Wextra -Werror -pthread -I includes
NAME    := codexion

# --------------------------------------------------------------------------- #
# Mandatory part sources
# --------------------------------------------------------------------------- #
SRCS    := src/main.c \
		   src/parser.c \
		   src/logs.c \
		   src/utils.c \
		   src/init.c \
		   src/heap.c \
		   src/dongle.c \
		   src/coder.c \
		   src/monitor.c \

OBJS    := $(SRCS:.c=.o)

# --------------------------------------------------------------------------- #
# Rules
# --------------------------------------------------------------------------- #

all: $(NAME)

$(NAME): $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)


# --------------------------------------------------------------------------- #
# Tester (Not mandatory, this tests the norms and leeks on the project)
# --------------------------------------------------------------------------- #

# --------------------------------------------------------------------------- #
# Tester (Not mandatory, this tests the norms and leeks on the project)
# --------------------------------------------------------------------------- #

ARGS_MULTI_STABLE := 5 1500 200 200 200 3 100 fifo
ARGS_SINGLE_BURNOUT := 1 800 200 200 200 2 100 fifo

test: all
	@echo "🚀 Running full test suite..."
	@echo ""
	@echo "🔍 [1/4] Norm check..."
	@if norminette src/*.c includes/*.h > /tmp/norm.log 2>&1; then \
		echo "✅ Norm OK"; \
	else \
		echo "❌ Norm errors:"; cat /tmp/norm.log; \
	fi
	@echo ""
	@echo "🧪 [2/4] Valgrind (leak-check, multiple coders, stable)..."
	@if valgrind --leak-check=full --error-exitcode=1 -q \
		./$(NAME) $(ARGS_MULTI_STABLE) > /tmp/valgrind.log 2>&1; then \
		echo "✅ No leaks detected"; \
	else \
		echo "❌ Valgrind found issues:"; cat /tmp/valgrind.log; \
	fi
	@echo ""
	@echo "🧪 [3/4] Valgrind (leak-check, single coder burnout path)..."
	@if valgrind --leak-check=full --error-exitcode=1 -q \
		./$(NAME) $(ARGS_SINGLE_BURNOUT) > /tmp/valgrind_single.log 2>&1; then \
		echo "✅ No leaks detected"; \
	else \
		echo "❌ Valgrind found issues:"; cat /tmp/valgrind_single.log; \
	fi
	@echo ""
	@echo "🧵 [4/4] Helgrind (race detection, multiple coders, stable)..."
	@valgrind --tool=helgrind -q \
		./$(NAME) $(ARGS_MULTI_STABLE) > /tmp/helgrind.log 2>&1; \
	if grep -q "Possible data race" /tmp/helgrind.log; then \
		echo "❌ Helgrind found issues:"; cat /tmp/helgrind.log; \
	else \
		echo "✅ No races detected"; \
	fi
	@echo ""
	@echo "🏁 Test suite finished."

# --------------------------------------------------------------------------- #
# Clean rules
# --------------------------------------------------------------------------- #
clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

# --------------------------------------------------------------------------- #
.PHONY: all clean fclean re test

