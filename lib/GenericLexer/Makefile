SRC_DIRS = .
SRC := $(shell find $(SRC_DIRS) -name '*.cpp')

OBJ = $(SRC:%.cpp=%.o)
NAME = liblexer

ifeq ($(BUILD),debug)
# "Debug" build - no optimization, and debugging symbols
CFLAGS += -O0 -g3 -std=c++17
CFLAGS += -DDEBUG
else
# "Release" build - optimization, and no debug symbols
CFLAGS += -O3 -s -DNDEBUG
endif

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
		@cp $(NAME).a ../

%.o : %.cpp
	@gcc $(CFLAGS) -Iinclude -W -Wall -c $< -o $@

.PHONY: clean
clean:
		@echo "$(bold)$(blue)Cleaning$(white) $(NAME)$(reset)"
		@$(RM) -r $(OBJ)

fclean: clean
		@$(RM) -r $(NAME).a
		@$(RM) -r ../$(NAME).a

re: fclean all
