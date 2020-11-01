#include "library/os.h"
#include <iostream>
#include <stdexcept>
#include <string.h>
#include "foxely.h"


Value whichNative(VM* oVM, int argCount, Value* args)
{
    Fox_FixArity(oVM, argCount, 0);
    #ifdef _WIN32
        return Fox_StringToValue(oVM, "windows");
    #elif __linux__
        return Fox_StringToValue(oVM, "linux");
    #elif __APPLE__
        #include "TargetConditionals.h"
        #if TARGET_OS_MAC
            return Fox_StringToValue(oVM, "macOS");
        #endif
    #endif
    return Fox_StringToValue(oVM, "Unknown OS");
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
    return Fox_StringToValue(oVM, getenv(Fox_ValueToCString(args[0])));
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
    Value instance = Fox_DefineInstanceOf(oVM, "Array");
    Fox_SetInstanceField(oVM, instance, "m_oArray", Fox_Array());
    if (!Fox_Is(instance, VAL_NIL))
    {
        Value arrayField = Fox_GetInstanceField(oVM, instance, "m_oArray");
        ObjectArray* array = Fox_ValueToArray(arrayField);
        std::vector<Value> values;

        for (int i = 1; i < oVM->argc; i++) {
            values.push_back(Fox_StringToValue(oVM, oVM->argv[i]));
        }

        array->m_vValues = values;
    }
    else
        Fox_RuntimeError(oVM, "Array Library was not imported.");
    return instance;
}



OSPlugin::OSPlugin(VM* oVM) : fox::pluga::IModule(oVM)
{
    // std::cout << "OSPlugin: Create" << std::endl;

	NativeMethods methods =
	{
		std::make_pair<std::string, NativeFn>("which", whichNative),
        std::make_pair<std::string, NativeFn>("shell", shellNative),
		std::make_pair<std::string, NativeFn>("getenv", getEnvNative),
		std::make_pair<std::string, NativeFn>("exit", exitNative),
		std::make_pair<std::string, NativeFn>("args", argsNative),
	};

	m_oMethods = methods;
}

OSPlugin::~OSPlugin()
{
    // std::cout << "OSPlugin: Destroy" << std::endl;
}

const char* OSPlugin::GetClassName() const
{
    return "os";
}