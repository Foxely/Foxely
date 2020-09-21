#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>

#include "chunk.h"
#include "debug.h"
#include "vm.hpp"

void repl()
{
    VM vm;
    char line[1024];
    for (;;)
    {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        vm.interpret(line);
    }
}

char* readFile(const char* path)
{
    FILE* file = fopen(path, "rb");

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(fileSize + 1);
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

void runFile(const char* path)
{
    VM vm;
    char* source = readFile(path);
    InterpretResult result = vm.interpret(source);
    free(source); 

    if (result == INTERPRET_COMPILE_ERROR) exit(65);
    if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int ac, char** av)
{
    VM vm;
    Chunk chunk;
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