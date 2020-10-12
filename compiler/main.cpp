#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>
#include <streambuf>
#include <memory>
#include <unistd.h>
#include <chrono>

#include "common.h"
#include "debug.h"
#include "Parser.h"
#include "ast.hpp"
#include "LinearAllocator.h"
#include "PointerMath.h"

void CheckError(std::list<AstNode*> root)
{
    for (auto& it : root)
    {
        switch (it->type)
        {
        case AST_Return:
            break;
        
        default:
            break;
        }
    }
}

std::string garbageCFile = "tiny_garbage/my_gc_new.c "
                            "tiny_garbage/gc_free.c "
                            "tiny_garbage/gc_malloc.c "
                            "tiny_garbage/gc_run.c "
                            "tiny_garbage/gc_stop.c "
                            "tiny_garbage/mark.c "
                            "tiny_garbage/push_pop.c "
                            "tiny_garbage/sweep.c";
std::string mainEnd = "";

int main(int ac, char** av, char** env)
{
    auto startCompiler = std::chrono::high_resolution_clock::now();
    if (ac >= 2)
    {
        std::cout << "========================\n";
        std::cout << "======= Compiler =======\n";
        std::cout << "========================\n\n";

        const char* path = av[1];
        std::string outCFile = "out.c";
        std::string outFile = "a.out";

        for (int i = 0; i < ac; i++)
        {
            if (!std::strcmp(av[i], "-o"))
                outFile = av[++i];

            if (!std::strcmp(av[i], "-v")) {
                VERBOSE_AST_TO_C = true;
                VERBOSE_TOKEN = true;
                VERBOSE_AST = true;
            }

            if (!std::strcmp(av[i], "D_TOKEN")) {
                VERBOSE_TOKEN = true;
            }

            if (!std::strcmp(av[i], "D_ASTC")) {
                VERBOSE_AST_TO_C = true;
            }

            if (!std::strcmp(av[i], "D_AST")) {
                VERBOSE_AST = true;
            }

            if (!std::strcmp(av[i], "build")) {
                BUILD = true;
            }
        }

        std::cout << "source file: " << path << "\n";
        std::cout << "C code output file: " << outCFile << "\n";
        std::cout << "exe file: " << outFile << "\n\n";

        std::ifstream t(path);
        if (!t.is_open())
        {
            std::cout << path << ": File doesn't exist.\n";
            return 84;
        }
        std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        t.close();

        std::ofstream stream(outCFile);

        size_t lMemorySize = 107374182; //107MB
        void* pProgramMemory = malloc(lMemorySize);
        Parser parser;

        if(pProgramMemory == nullptr)
        {
            std::cout << "Allocation Memory Failed" << std::endl;
            return 84;
        }

        parser.allocator = new (pProgramMemory)LinearAllocator(lMemorySize - sizeof(LinearAllocator),
                                                        PointerMath::Add(pProgramMemory, sizeof(LinearAllocator)));

        if (!CreateAst(parser, str.c_str()))
            return 84;
        
        CheckError(parser.pRoot);
        
        for (auto& it : parser.pRoot)
            if (VERBOSE_AST_TO_C)
                it->visit(std::cout, 0);
            else
                it->visit(stream, 0);
        
        free(pProgramMemory);
        stream.close();

        if (BUILD)
        {
            std::string commandGCC = "sudo gcc -O3 " + std::string(outCFile) + " -IFoxely_C_include " + garbageCFile + " -o " + outFile;
            std::string commandRM = "rm " + std::string(outCFile);

            std::cout << "Compilation Cmd: " << commandGCC << std::endl;
            system(commandGCC.c_str());
            system(commandRM.c_str());
        }

        auto endCompiler = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = endCompiler - startCompiler;

        std::cout << "\nCompiler Time: " << elapsed.count() << " ms" << std::endl;
    }
    return 0;
}
