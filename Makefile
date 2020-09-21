##
## EPITECH PROJECT, 2019
## Makefile
## File description:
## make an executable
##

CC = g++

EXECUTABLE	= main
SRC_DIR = src


MAIN_SRC = main.cpp
MAIN_OBJ = $(MAIN_SRC:%.cpp=%.o)
SRC = $(shell find $(SRC_DIR) -name '*.cpp')
OBJ = $(SRC:%.cpp=%.o)

CFLAGS += -std=c++17 -W -Wall -Wextra $(if $(DEBUG),-g3) $(if $(DEBUG),-DDEBUG -DDEBUG_TRACE_EXECUTION)
LDFLAGS = -Llib -llexer
INC_FLAGS = -Iinclude -Ilib/GenericLexer/include

all: lib_make bin/$(EXECUTABLE)

lib_make:
	make -C lib/GenericLexer/

run: lib_make all
	@./bin/$(EXECUTABLE) $(ARGS)

bin/$(EXECUTABLE): $(MAIN_OBJ) $(OBJ)
	@$(CC) -o $@ $(MAIN_OBJ) $(OBJ) $(CFLAGS) $(INC_FLAGS) $(LDFLAGS)

%.o: %.cpp
	@$(CC) $(INC_FLAGS) $(CFLAGS) -c $< -o $@
	@echo "\033[1;32mCompiled \033[1;37m'$<'\033[m"

.PHONY: clean
clean:
	@$(RM) -r $(OBJ)

.PHONY: fclean
fclean: clean
	@$(RM) -r $(BIN)/$(EXECUTABLE)
	@$(RM) -r vgcore*

.PHONY: re
re: fclean all

valgrind:
	valgrind --leak-check=full -s ./bin/$(EXECUTABLE) $(ARGS)
