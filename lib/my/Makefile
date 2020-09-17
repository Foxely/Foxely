##
## EPITECH PROJECT, 2019
## make lib
## File description:
## it makes files for the lib
##

SRC_DIRS ?= .
SRC := $(shell find $(SRC_DIRS) -name '*.c')

OBJ = $(SRC:%.c=%.o)
NAME = libmy.a

bold = `tput bold`
red = `tput setaf 1`
green = `tput setaf 2`
yellow = `tput setaf 3`
blue = `tput setaf 4`
magenta = `tput setaf 5`
cyan = `tput setaf 6`
white = `tput setaf 7`
reset = `tput sgr0`

all: $(NAME)

$(NAME): $(OBJ)
		@echo "$(bold)$(magenta)Compiling$(white) $(NAME) $(reset)"
		@ar -rc $(NAME) $(OBJ)
		@cp $(NAME) ../

%.o : %.c
	@gcc -I../include -W -Wall -c $< -o $@ -g3 $(if $(DEBUG),-g3) $(if $(DEBUG),-DDEBUG)

.PHONY: clean
clean:
		@echo "$(bold)$(blue)Cleaning$(white) $(NAME)$(reset)"
		@$(RM) -r $(OBJ)

fclean: clean
		@$(RM) -r $(NAME)
		@$(RM) -r ../$(NAME)

re: fclean all