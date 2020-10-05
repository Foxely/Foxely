
SRC = sfml.cpp
LIBNAME = sfml

all:
	@g++ -Wall -fPIC -shared -I./ -I../../include -I../../lib/GenericLexer/include -LSFML-2.5.1/lib -lsfml-graphics -lsfml-window -lsfml-system $(SRC) -o $(LIBNAME).so -ISFML-2.5.1/include
