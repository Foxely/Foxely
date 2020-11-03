##
## EPITECH PROJECT, 2019
## Makefile
## File description:
## make an executable
##

CC = g++ -std=c++17

NAME	= foxely
SRC_DIR = src

SRC = $(shell find $(SRC_DIR) -name '*.cpp')
OBJ = $(SRC:%.cpp=%.o)

# CFLAGS += -std=c++17 -W -Wall -Wextra $(if $(DEBUG),-g3) $(if $(DEBUG),-DDEBUG -DDEBUG_TRACE_EXECUTION)
# CFLAGS += $(if $(DEBUG_TOKEN),-DDEBUG_TOKEN)
LDFLAGS = -Llib -llexer -ldl -lstdc++fs -lpthread
INC_FLAGS = -Iinclude -Ilib/GenericLexer/include

ifeq ($(BUILD),debug)
# "Debug" build - no optimization, and debugging symbols
CFLAGS += -O0 -g3
CFLAGS += -DDEBUG -DDEBUG_TRACE_EXECUTION
CFLAGS += $(if $(DEBUG_TOKEN),-DDEBUG_TOKEN)
CFLAGS += $(if $(DEBUG_LOG_GC),-DDEBUG_LOG_GC)
else
# "Release" build - optimization, and no debug symbols
CFLAGS += -O3 -s
endif

all: lib_make lib

lib_make:
	@make -sC lib/GenericLexer/

lib: $(OBJ)
	@ar -rc lib$(NAME).a $(OBJ)

%.o: %.cpp
	@$(CC) $(INC_FLAGS) $(CFLAGS) -c $< -o $@
	@echo "\033[1;32mCompiled \033[1;37m'$<'\033[m"

.PHONY: clean
clean:
	@$(RM) -r $(OBJ)
	@$(RM) -r $(MAIN_OBJ)

.PHONY: fclean
fclean: clean
	@$(RM) -r lib$(NAME).a
	@$(RM) -r vgcore*
	@make fclean -sC lib/GenericLexer

.PHONY: re
re: fclean all