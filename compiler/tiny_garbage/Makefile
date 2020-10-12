#
# EPITECH PROJECT, 2019
# make lib
# File description:
# it makes files for the lib
#

DIR = .

SRC =	gc_free.c \
		gc_malloc.c \
		gc_run.c \
		gc_stop.c \
		mark.c \
		my_gc_new.c \
		push_pop.c \
		sweep.c

OBJ = $(SRC:.c=.o)
NAME = libgc

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
		@ar -rc $(NAME).a $(OBJ)
		@mv $(NAME).a ../

%.o : %.c
	@gcc -I ../include/ -L ../ -lmy -W -Wall -c $< -o $@

.PHONY: clean
clean:
		@echo "$(bold)$(blue)Cleaning$(white) $(NAME)$(reset)"
		@rm -f $(OBJ)

fclean: clean
		@rm -f ../$(NAME).a

re: fclean all