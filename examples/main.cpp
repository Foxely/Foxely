#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <streambuf>
#include <unistd.h>
#include <termios.h>
#include <sstream>
#include <algorithm>

#include "library/library.h"
#include "foxely.h"


char* RED = "\u001b[31m";
char* BLACK = "\u001b[30m";
char* GREEN = "\u001b[32m";
char* YELLOW = "\u001b[33m";
char* BLUE = "\u001b[34m";
char* MAGENTA = "\u001b[35m";
char* CYAN = "\u001b[36m";
char* WHITE = "\u001b[37m";

char* Bright_Black = "\u001b[30;1m";
char* Bright_Red = "\u001b[31;1m";
char* Bright_Green = "\u001b[32;1m";
char* Bright_Yellow = "\u001b[33;1m";
char* Bright_Blue = "\u001b[34;1m";
char* Bright_Magenta = "\u001b[35;1m";
char* Bright_Cyan = "\u001b[36;1m";
char* Bright_White = "\u001b[37;1m";

char* Bold = "\u001b[1m";
char* Underline = "\u001b[4m";
char* Reversed = "\u001b[7m";


char* Up = "\u001b[{n}A";
char* Down = "\u001b[{n}B";
char* Right = "\u001b[{n}C";
char* Left = "\u001b[1D";


char* RESET = "\u001b[0m";


void repl()
{
    VM oVM;

    std::string input;
    std::string line;
    std::cout << RESET;
    while(true)
    {
        input.clear();
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input[input.size() - 1] == '{') {
            std::cout << "| ";
            while (std::getline(std::cin, line))
            {
                if (line == "}") {
                    input += line;
                    break;
                }
                std::cout << "| ";
                input += line;
            }
            std::cout << "\n";
        }

        if (input == "exit")
            break;

        if (input[0] != '\n' && input[0] != '\0')
            oVM.Interpret(NULL, input.c_str());
        else {
            std::cout << std::endl;
            break;
        }
    }
}

struct Test
{
    int a;
};

NativeMethods testMethods =
{
	std::make_pair<std::string, NativeFn>("init", [](VM* pVM, int argc, Value* args)
    {
        ObjectInstance* pInstance = Fox_AsInstance(args[-1]);
        pInstance->cStruct = new Test;
        Fox_SetField(pVM, Fox_Object(pInstance), "x", Fox_Double(0));
        
        Fox_Setter(pVM, pInstance, "x", [](VM* pVM, int argc, Value* args)
        {
            Test* pTest = (Test *) Fox_GetUserData(args[-1]);
            pTest->a = Fox_AsInt(args[0]);
            return Fox_Int(pTest->a);
        });
        Fox_Getter(pVM, pInstance, "x", [](VM* pVM, int argc, Value* args)
        {
            Test* pTest = (Test *) Fox_GetUserData(args[-1]);
            return Fox_Int(pTest->a);
        });
        return args[-1];
    }),
};

void runFile(int ac, char** av, const char* path)
{
    VM oVM;

    oVM.argc = ac;
    oVM.argv = av;
	std::ifstream t(path);
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    t.close();

    Test test;

    oVM.DefineFunction("core", "clock", clockNative);
    oVM.DefineModule("main");
    oVM.DefineClass("main", "Test", testMethods);
    DefineIOModule(&oVM);
    DefineOSModule(&oVM);
    DefineMathModule(&oVM);
    DefineModuleModule(&oVM);
    DefinePathModule(&oVM);

    InterpretResult result = INTERPRET_OK;
    result = oVM.Interpret("main", str.c_str());

    // Callable say = oVM.Function("main", "sayHello()");
    // if (say.IsValid())
        // say.Call();

    // Callable add = oVM.Function("main", "add(_,_)");
    // if (add.IsValid())
    //     add.Call(1, 2);

    if (result == INTERPRET_COMPILE_ERROR) exit(65);
    if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int ac, char** av)
{
    if (ac == 1) {
        IsRepl = true;
        repl();
    } else if (ac >= 2) {
        runFile(ac, av, av[1]);
    } else {
        fprintf(stderr, "Usage: foxely [path]\n");
        exit(64);
    }
    return 0;
}
