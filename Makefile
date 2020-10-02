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

# CFLAGS += -std=c++17 -W -Wall -Wextra $(if $(DEBUG),-g3) $(if $(DEBUG),-DDEBUG -DDEBUG_TRACE_EXECUTION)
# CFLAGS += $(if $(DEBUG_TOKEN),-DDEBUG_TOKEN)
LDFLAGS = -Llib -llexer -ldl
INC_FLAGS = -Iinclude -Ilib/GenericLexer/include

ifeq ($(BUILD),debug)
# "Debug" build - no optimization, and debugging symbols
CFLAGS += -O0 -g3 -std=c++17
CFLAGS += -DDEBUG -DDEBUG_TRACE_EXECUTION
CFLAGS += $(if $(DEBUG_TOKEN),-DDEBUG_TOKEN)
CFLAGS += $(if $(DEBUG_LOG_GC),-DDEBUG_LOG_GC)
else
# "Release" build - optimization, and no debug symbols
CFLAGS += -O3 -s -DNDEBUG
endif

all: lib_make bin/$(EXECUTABLE)

lib_make:
	@make -sC lib/GenericLexer/

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
	@$(RM) -r $(MAIN_OBJ)

.PHONY: fclean
fclean: clean
	@$(RM) -r bin/$(EXECUTABLE)
	@$(RM) -r vgcore*
	@make fclean -sC lib/GenericLexer

.PHONY: re
re: fclean all

valgrind:
	valgrind --leak-check=full -s ./bin/$(EXECUTABLE) $(ARGS)

debug:
	@make "BUILD=debug" -sC lib/GenericLexer
	@make "BUILD=debug"

alls:
	g++ -Iinclude tests/plugatests.cpp -ldl
