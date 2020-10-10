#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>
#include <streambuf>
#include <memory>

#include "chunk.hpp"
#include "debug.h"
#include "Parser.h"
#include "ast.hpp"
#include "LinearAllocator.h"
#include "PointerMath.h"

void repl()
{
    char line[1024] = "";
    for (;;)
    {
        printf("> ");

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
    }
}

void runFile(const char* path)
{
	std::ifstream t(path);
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    t.close();

    size_t lMemorySize = 107374182; //107MB
    void* pProgramMemory = malloc(lMemorySize);
    Parser parser;
    Chunk chunk;

    if(pProgramMemory == nullptr)
    {
        std::cout << "Allocation Memory Failed" << std::endl;
        return;
    }

    parser.allocator = new (pProgramMemory)LinearAllocator(lMemorySize - sizeof(LinearAllocator),
                                                    PointerMath::Add(pProgramMemory, sizeof(LinearAllocator)));

    if (!Compile(parser, str.c_str(), &chunk))
        return;
    else
        for (auto& it : parser.pRoot)
            it->visit(std::cout);
    free(pProgramMemory);
}

int main(int ac, char** av)
{
    if (ac >= 2)
    {
        bool verbose = false;
        const char* path = av[1];
        std::string outCFile = "a.c";
        std::string outFile = "a.out";

        for (int i = 0; i < ac; i++)
        {
            if (!std::strcmp(av[i], "-o"))
            {
                outFile = av[++i];
            }

            if (!std::strcmp(av[i], "-v"))
            {
                verbose = true;
            }
        }



        std::ifstream t(path);
        std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        t.close();

        std::ofstream stream(outCFile);

        size_t lMemorySize = 107374182; //107MB
        void* pProgramMemory = malloc(lMemorySize);
        Parser parser;
        Chunk chunk;

        if(pProgramMemory == nullptr)
        {
            std::cout << "Allocation Memory Failed" << std::endl;
            return 84;
        }

        parser.allocator = new (pProgramMemory)LinearAllocator(lMemorySize - sizeof(LinearAllocator),
                                                        PointerMath::Add(pProgramMemory, sizeof(LinearAllocator)));
        if (Compile(parser, str.c_str(), &chunk))
            for (auto& it : parser.pRoot)
                if (verbose)
                    it->visit(std::cout);
                else
                    it->visit(stream);
        free(pProgramMemory);
        stream.close();

        if (!verbose)
        {
            std::string commandGCC = "gcc -O3 " + std::string(outCFile) + std::string(" -o ") + outFile;
            std::string commandRM = "rm " + std::string(outCFile);
            system(commandGCC.c_str());
            system(commandRM.c_str());
        }

    }
    return 0;
}
