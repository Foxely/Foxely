#include <iostream>
#include <string.h>
#include <algorithm>

#include "math.h"
#include "library/library.h"
#include "foxely.h"

Value sinNative(VM* oVM, int argCount, Value* args)
{
    Fox_FixArity(oVM, argCount, 1);
    double value = 0;
    if (Fox_IsDouble(args[0]))
    {
        value = sin(Fox_AsDouble(args[0]));
    }
    return (Fox_Double(value));
}

Value cosNative(VM* oVM, int argCount, Value* args)
{
    Fox_FixArity(oVM, argCount, 1);
    double value = 0;
    if (Fox_IsDouble(args[0]))
    {
        value = cos(Fox_AsDouble(args[0]));
    }
    return (Fox_Double(value));
}

void DefineMathModule(VM* oVM)
{
    NativeMethods methods =
	{
		std::make_pair<std::string, NativeFn>("sin", sinNative),
		std::make_pair<std::string, NativeFn>("cos", cosNative),
	};
    oVM->DefineModule("Math");
    oVM->DefineClass("Math", "Math", methods);
}