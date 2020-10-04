#include "library/os.h"
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <filesystem>
#include "foxely.h"
namespace fs = std::filesystem;

// SCY_PLUGIN(OSPlugin, "OS Module", "0.1.0")


Value whichNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
    #ifdef _WIN32
        return Fox_StringToValue("windows");
    #elif __linux__
        return Fox_StringToValue("linux");
    #elif __APPLE__
        #include "TargetConditionals.h"
        #if TARGET_OS_MAC
            return Fox_StringToValue("macOS");
        #endif
    #endif
    return Fox_StringToValue("Unknown OS");
}

Value shellNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 1);
    if (argCount == 1)
        system(Fox_ValueToCString(args[0]));
    return NIL_VAL;
}

Value getEnvNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 1);
    return Fox_StringToValue(getenv(Fox_ValueToCString(args[0])));
}

Value exitNative(int argCount, Value* args)
{
    Fox_Arity(argCount, 0, 1);
    if (argCount == 1)
    {
        if (Fox_Is(args[0], VAL_NUMBER))
            exit(Fox_ValueToNumber(args[0]));
        else
            Fox_RuntimeError("Expected number");
    }
    else
        exit(0);
    return NIL_VAL;
}

Value argsNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
    Value instance = Fox_DefineInstanceOf("Array");
    Fox_SetInstanceField(instance, "m_oArray", Fox_Array());
    if (!Fox_Is(instance, VAL_NIL))
    {
        Value arrayField = Fox_GetInstanceField(instance, "m_oArray");
        ObjectArray* array = Fox_ValueToArray(arrayField);
        std::vector<Value> values;

        for (int i = 1; i < VM::GetInstance()->argc; i++) {
            values.push_back(Fox_StringToValue(VM::GetInstance()->argv[i]));
        }

        array->m_vValues = values;
    }
    else
        Fox_RuntimeError("Array Library was not imported.");
    return instance;
}

// Value Native(int argCount, Value* args)
// {
//     std::string path = "/path/to/directory";
//     for (const auto & entry : fs::directory_iterator(path))
//         std::cout << entry.path() << std::endl;
// }




OSPlugin::OSPlugin()
{
    // std::cout << "OSPlugin: Create" << std::endl;
}

OSPlugin::~OSPlugin()
{
    // std::cout << "OSPlugin: Destroy" << std::endl;
}

const char* OSPlugin::GetClassName() const
{
    return "os";
}

NativeMethods OSPlugin::GetMethods()
{
	NativeMethods methods =
	{
		std::make_pair<std::string, NativeFn>("which", whichNative),
        std::make_pair<std::string, NativeFn>("shell", shellNative),
		std::make_pair<std::string, NativeFn>("getenv", getEnvNative),
		std::make_pair<std::string, NativeFn>("exit", exitNative),
		std::make_pair<std::string, NativeFn>("args", argsNative),
	};
    
	return methods;
}