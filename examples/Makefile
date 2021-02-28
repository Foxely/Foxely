##
## EPITECH PROJECT, 2019
## Makefile
## File description:
## make an executable
##

CC = g++ -std=c++17

EXECUTABLE	= foxely


MAIN_SRC = main.cpp
MAIN_OBJ = $(MAIN_SRC:%.cpp=%.o)
SRC = $(shell find "lib/MeCli/src/" -name '*.cpp')
OBJ = $(SRC:%.cpp=%.o)

# CFLAGS += -std=c++17 -W -Wall -Wextra $(if $(DEBUG),-g3) $(if $(DEBUG),-DDEBUG -DDEBUG_TRACE_EXECUTION)
# CFLAGS += $(if $(DEBUG_TOKEN),-DDEBUG_TOKEN)
LDFLAGS = -L../ -lfoxely -L../lib -llexer -lstdc++fs -ldl
INC_FLAGS = -I../include -I../lib/GenericLexer/include -Ilib/MeCli/include

ifeq ($(BUILD),debug)
# "Debug" build - no optimization, and debugging symbols
CFLAGS += -pthread -O0 -g3
CFLAGS += -DDEBUG -DDEBUG_TRACE_EXECUTION
CFLAGS += $(if $(DEBUG_TOKEN),-DDEBUG_TOKEN)
CFLAGS += $(if $(DEBUG_LOG_GC),-DDEBUG_LOG_GC)
else
# "Release" build - optimization, and no debug symbols
CFLAGS += -pthread -O3 -s -DNDEBUG
endif

all: bin/$(EXECUTABLE)

run: all
	@./bin/$(EXECUTABLE) $(ARGS)

bin/$(EXECUTABLE): lib $(MAIN_OBJ) $(OBJ)
	@$(CC) -export-dynamic -o $@ $(OBJ) $(MAIN_OBJ) $(CFLAGS) $(INC_FLAGS) $(LDFLAGS)

%.o: %.cpp
	@$(CC) $(INC_FLAGS) $(CFLAGS) -c $< -o $@
	@echo "\033[1;32mCompiled \033[1;37m'$<'\033[m"

.PHONY: clean
clean:
	@$(RM) -r $(MAIN_OBJ)

.PHONY: fclean
fclean: clean
	@$(RM) -r bin/$(EXECUTABLE)
	@$(RM) -r vgcore*

.PHONY: re
re: fclean all

valgrind:
	valgrind --leak-check=full -s ./bin/$(EXECUTABLE) $(ARGS)

debug:
	@make "BUILD=debug"