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
		   src/scheduler.c \

OBJS    := $(SRCS:.c=.o)

# --------------------------------------------------------------------------- #
# Rules
# --------------------------------------------------------------------------- #

all: $(NAME)

$(NAME): $(OBJS) $(LIBFT_A) $(PRINTF_A)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)


# --------------------------------------------------------------------------- #
# Tester (Not mandatory, this tests the norms and leeks on the project)
# --------------------------------------------------------------------------- #

ARGS := 5 800 200 200 200 3 100 fifo

test: all
	@echo "🚀 Running full test suite..."
	@echo ""
	@echo "🔍 [1/3] Norm check..."
	@if norminette src/*.c includes/*.h > /tmp/norm.log 2>&1; then \
		echo "✅ Norm OK"; \
	else \
		echo "❌ Norm errors:"; cat /tmp/norm.log; \
	fi
	@echo ""
	@echo "🧪 [2/3] Valgrind (leak-check)..."
	@if valgrind --leak-check=full --error-exitcode=1 -q \
		./$(NAME) $(ARGS) > /tmp/valgrind.log 2>&1; then \
		echo "✅ No leaks detected"; \
	else \
		echo "❌ Valgrind found issues:"; cat /tmp/valgrind.log; \
	fi
	@echo ""
	@echo "🧵 [3/3] Helgrind (race detection)..."
	@if valgrind --tool=helgrind --error-exitcode=1 -q \
		./$(NAME) $(ARGS) > /tmp/helgrind.log 2>&1; then \
		echo "✅ No races detected"; \
	else \
		echo "❌ Helgrind found issues:"; cat /tmp/helgrind.log; \
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

