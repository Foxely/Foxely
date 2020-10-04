#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>
#include <streambuf>

#include "chunk.hpp"
#include "debug.h"
#include "Parser.h"
#include "vm.hpp"

void repl()
{
    GC::Gc.pVm = VM::GetInstance();
    char line[1024] = "";
    for (;;)
    {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        VM::GetInstance()->interpret(line);
    }
}

void runFile(const char* path)
{
    GC::Gc.pVm = VM::GetInstance();
	std::ifstream t(path);
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

    InterpretResult result = VM::GetInstance()->interpret(str.c_str());

    if (result == INTERPRET_COMPILE_ERROR) exit(65);
    if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int ac, char** av)
{
    VM::GetInstance()->argc = ac;
    VM::GetInstance()->argv = av;
    if (ac == 1) {
        repl();
    } else if (ac == 2) {
        runFile(av[1]);
    } else {
        fprintf(stderr, "Usage: clox [path]\n");
        exit(64);
    }
    return 0;
}
