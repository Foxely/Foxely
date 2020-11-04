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
    if (Fox_Is(args[0], VAL_NUMBER))
    {
        value = sin(Fox_ValueToNumber(args[0]));
    }
    return (NUMBER_VAL(value));
}

Value cosNative(VM* oVM, int argCount, Value* args)
{
    Fox_FixArity(oVM, argCount, 1);
    double value = 0;
    if (Fox_Is(args[0], VAL_NUMBER))
    {
        value = cos(Fox_ValueToNumber(args[0]));
    }
    return (NUMBER_VAL(value));
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