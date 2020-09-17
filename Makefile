##
## EPITECH PROJECT, 2019
## Makefile
## File description:
## make an executable
##

CC = gcc
TARGET_EXEC ?= foxely

SRC_DIRS = ./interpreter

SRC = $(shell find $(SRC_DIRS) -name '*.c')
MAIN = main.c
OBJS = $(MAIN:%.c=%.o) $(SRC:%.c=%.o)
CFLAGS += -W -Wall $(if $(DEBUG),-g3 -D DEBUG_TRACE_EXECUTION)
CFLAGS += $(if $(DEBUG_MEMORY), -D DEBUG_LOG_GC -D DEBUG_STRESS_GC -D DEBUG_PRINT_CODE)
LDFLAGS = -Llib -lmy -lm -ldl -lcsfml-graphics -lcsfml-window -lcsfml-system -lcsfml-audio
LD = $(shell find lib -name '*.c')
INC_FLAGS = -Iinclude -Ilib/include

TEST_NAME = unit_tests
UNIT_SRC =	$(shell find asm -name '*.c')


TEST_SRC =	$(shell find tests -name '*.c')




all: lib_make $(TARGET_EXEC)

lib_make:
	@make -sC lib/my

$(TARGET_EXEC): $(OBJS)
	@$(CC) -o $@ $(OBJS) $(CFLAGS) $(LDFLAGS)

%.o: %.c
	@$(CC) $(INC_FLAGS) $(CFLAGS) -c $< -o $@
	@echo "\033[1;32mCompiled \033[1;37m'$<'\033[m"

.PHONY: clean
clean:
	@$(RM) -r main.o
	@$(RM) -r $(OBJS)
	@$(RM) -r *.gcno
	@$(RM) -r *.gcda
	@$(RM) -r "NUL"

.PHONY: fclean
fclean: clean
	@$(RM) -r $(TARGET_EXEC)
	@$(RM) -r $(TEST_NAME)
	@$(RM) -r vgcore*
	@make fclean -sC lib/my

.PHONY: re
re: fclean all

valgrind:
	valgrind --leak-check=full -s ./$(NAME)

tests_run: lib_make
	@echo "\033[1;95mRunning tests...\033[0;39m"
	@gcc -o $(TEST_NAME) $(UNIT_SRC) $(TEST_SRC) $(INC_FLAGS) $(CFLAGS) $(LDFLAGS) --coverage -lcriterion
	./$(TEST_NAME)
	gcovr
	@echo "\033[1;94mTest finished, here are the results\033[0;39m"
	@make fclean

csfml:
	@gcc -c csfml.c -lcsfml-graphics -lcsfml-window -lcsfml-system -lcsfml-audio $(INC_FLAGS)
	@ar -rc lib/libcsfml.a csfml.o