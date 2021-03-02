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
#include "App.hpp"

void highlight(const std::string& str)
{
    std::cout << mecli::style::bold;
    std::cout << mecli::style::green;
    std::cout << str << mecli::style::reset;
}

void repl(int ac, char** av)
{
    VM oVM(ac, av);
    mecli::App app;

    app.SetOnLineListener([&oVM](const std::string& str)
    {
        oVM.Interpret(NULL, str.c_str());
    });

    app.AddSyntax("for", highlight);
    app.AddSyntax("while", highlight);
    app.AddSyntax("class", highlight);
    app.AddSyntax("func", highlight);

    app.Run();

    // std::string input;
    // std::string line;
    // while(true)
    // {
    //     input.clear();
    //     std::cout << "> ";
    //     std::getline(std::cin, input);

    //     if (input[input.size() - 1] == '{') {
    //         std::cout << "| ";
    //         int refCount = 1;
    //         while (std::getline(std::cin, line))
    //         {
    //             if (line[line.size() - 1] == '{')
    //                 refCount++;
    //             if (line == "}")
    //                 refCount--;
    //             if (refCount <= 0)
    //             {
    //                 input += line;
    //                 break;
    //             }
    //             std::cout << "| ";
    //             input += line;
    //         }
    //         std::cout << "\n";
    //     }

    //     if (input == "exit")
    //         break;

    //     if (input[0] != '\n' && input[0] != '\0')
    //         oVM.Interpret(NULL, input.c_str());
    //     else {
    //         std::cout << std::endl;
    //         break;
    //     }
    // }
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

void runFile(int ac, char** av, const std::string& path)
{
    VM oVM(ac, av);
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
    std::string strFile;
    for (int i = 1; i < ac; ++i)
    {
        if (av[i][0] != '-')
        {
            strFile = av[i];
        }
    }
    if (strFile.empty()) {
        IsRepl = true;
        repl(ac , av);
    } else if (ac >= 2) {
        runFile(ac, av, strFile);
    } else {
        fprintf(stderr, "Usage: foxely [path]\n");
        exit(64);
    }
    return 0;
}
