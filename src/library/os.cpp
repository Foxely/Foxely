#include <iostream>
#include <stdexcept>
#include <string.h>

#include "library/library.h"
#include "foxely.h"


Value whichNative(VM* oVM, int argCount, Value* args)
{
    Fox_FixArity(oVM, argCount, 0);
    #ifdef _WIN32
        return Fox_String(oVM, "windows");
    #elif __linux__
        return Fox_String(oVM, "linux");
    #elif __APPLE__
        #include "TargetConditionals.h"
        #if TARGET_OS_MAC
            return Fox_String(oVM, "macOS");
        #endif
    #endif
    return Fox_String(oVM, "Unknown OS");
}

Value shellNative(VM* oVM, int argCount, Value* args)
{
    Fox_FixArity(oVM, argCount, 1);
    if (argCount == 1)
        system(Fox_ValueToCString(args[0]));
    return NIL_VAL;
}

Value getEnvNative(VM* oVM, int argCount, Value* args)
{
    Fox_FixArity(oVM, argCount, 1);
    return Fox_String(oVM, getenv(Fox_ValueToCString(args[0])));
}

Value exitNative(VM* oVM, int argCount, Value* args)
{
    Fox_Arity(oVM, argCount, 0, 1);
    if (argCount == 1)
    {
        if (Fox_Is(args[0], VAL_NUMBER))
            exit(Fox_ValueToNumber(args[0]));
        else
            Fox_RuntimeError(oVM, "Expected number");
    }
    else
        exit(0);
    return NIL_VAL;
}

Value argsNative(VM* oVM, int argCount, Value* args)
{
        Fox_FixArity(oVM, argCount, 0);
    Value oArray = Fox_Array(oVM);
    ObjectArray* pArray = Fox_ValueToArray(oArray);
    std::vector<Value> vValues;

    for (int i = 1; i < oVM->argc; i++)
        vValues.push_back(Fox_String(oVM, oVM->argv[i]));

    pArray->m_vValues = vValues;
    return oArray;
}


void DefineOSModule(VM* oVM)
{
    NativeMethods methods =
	{
		std::make_pair<std::string, NativeFn>("which", whichNative),
        std::make_pair<std::string, NativeFn>("shell", shellNative),
		std::make_pair<std::string, NativeFn>("getenv", getEnvNative),
		std::make_pair<std::string, NativeFn>("exit", exitNative),
		std::make_pair<std::string, NativeFn>("args", argsNative),
	};

    oVM->DefineModule("os");
    oVM->DefineClass("os", "os", methods);
}