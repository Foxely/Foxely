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
}

struct Test
{
    Test() {
        std::cout << "COnstructor no Arguments" << std::endl;
        a = 10;
    }

    void sayHello()
    {
        std::cout << "Hello!" << std::endl;
    }

    void sayAge(int age)
    {
        std::cout << "My age is " << age << "!" << std::endl;
    }

    void between(int min, int max)
    {
        std::cout << "min: " << min << ", max: " << max << std::endl;
    }

    int test(int min, int max)
    {
        std::cout << "test(min: " << min << ", max: " << max << ")" << std::endl;
        return 45;
    }

    int a;

    int getA() const
    {
        std::cout << "Getter" << std::endl;
        return a;
    }

    void setA(int b)
    {
        std::cout << "Setter" << std::endl;
        a = b;
    }
};

void ret_void_no_param()
{
    std::cout << "Test!!!" << std::endl;
}

void ret_void_int_param(int v1)
{
    std::cout << "ret_void_int_param(" << v1 << ")" << std::endl;
}

void ret_void_int_param_int(int v1, int v2)
{
    std::cout << "ret_void_int_param_int(" << v1 << ", " << v2 << ")" << std::endl;
}

int ret_int_no_param()
{
    return 45;
}

int ret_int_int_param(int v1)
{
    std::cout << "ret_int_int_param(" << v1 << ")" << std::endl;
    return 45;
}

int ret_int_int_param_int(int v1, int v2)
{
    std::cout << "ret_int_int_param_int(" << v1 << ", " << v2 << ")" << std::endl;
    return 45;
}


void runFile(int ac, char** av, const std::string& path)
{
#if PROFILING
    Instrumentor::Get().BeginSession("runFile");
#endif
    VM oVM(ac, av);
	std::ifstream t(path);
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    t.close();

    auto& time_module = oVM.DefineModule("time");
    time_module.raw_func("clock", clockNative);

    auto& test_module = oVM.DefineModule("test");
    DefineIOModule(&oVM);
    DefineOSModule(&oVM);
    DefineMathModule(&oVM);
    DefineModuleModule(&oVM);
    DefinePathModule(&oVM);

    test_module.func("ret_void_no_param", &ret_void_no_param);
    test_module.func("ret_int_no_param", &ret_int_no_param);
    test_module.func("ret_void_int_param", &ret_void_int_param);
    test_module.func("ret_void_int_param_int", &ret_void_int_param_int);
    test_module.func("ret_int_int_param", &ret_int_int_param);
    test_module.func("ret_int_int_param_int", &ret_int_int_param_int);
    Klass<Test>* klass = test_module.klass<Test>("Test");

    klass->func("sayHello", &Test::sayHello);
    klass->func("sayAge", &Test::sayAge);
    klass->func("between", &Test::between);
    klass->func("test", &Test::test);

    klass->var("a", &Test::a);
    klass->prop("bar", &Test::getA, &Test::setA);

    InterpretResult result = INTERPRET_OK;
    result = oVM.Interpret("main", str.c_str());

    Callable say = oVM.Function("main", "sayHello()");
    if (say.IsValid())
        say.Call();

    Callable add = oVM.Function("main", "add(_,_)");
    if (add.IsValid())
        add.Call(1, 2);

    // if (result == INTERPRET_COMPILE_ERROR) exit(65);
    // if (result == INTERPRET_RUNTIME_ERROR) exit(70);
#if PROFILING
    Instrumentor::Get().EndSession();
#endif
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
