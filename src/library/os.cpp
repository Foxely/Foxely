#include "library/os.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <fstream>
#include "foxely.h"

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
    return Fox_StringToValue("nil");
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
	};
	return methods;
}