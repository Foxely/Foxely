#include "os.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include "foxely.h"

SCY_PLUGIN(OSPlugin, "OS Module", "0.1.0")


Value whichNative(int argCount, Value* args)
{
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
    system(Fox_ValueToCString(args[0]));
    return NIL_VAL;
}

Value getEnvNative(int argCount, Value* args)
{
    return Fox_StringToValue(getenv(Fox_ValueToCString(args[0])));
}



OSPlugin::OSPlugin()
{
    std::cout << "OSPlugin: Create" << std::endl;
}

OSPlugin::~OSPlugin()
{
    std::cout << "OSPlugin: Destroy" << std::endl;
}

const char* OSPlugin::GetClassName() const
{
    return "os";
}

std::map<std::string, std::pair<NativeFn, int>> OSPlugin::GetMethods() const
{
	NativeMethods methods =
	{
		std::make_pair<std::string, Native>("which", std::make_pair<NativeFn, int>(whichNative, 0)), 	std::make_pair<std::string, Native>("shell", std::make_pair<NativeFn, int>(shellNative, 1)),
		std::make_pair<std::string, Native>("getenv", std::make_pair<NativeFn, int>(getEnvNative, 1)),
	};
	return methods;
}