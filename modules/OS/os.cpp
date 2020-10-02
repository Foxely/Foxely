#include "os.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include "foxely.h"


//
// Test Plugin
//


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


bool OSPlugin::onCommand(const char* node, const char* data,
                           unsigned int size)
{
    std::cout << "OSPlugin: Command: " << node << ": " << data << std::endl;

    try {
        // Handle a JSON encoded options hash
        if (strcmp(node, "options:set") == 0) {
            std::cout << "Launching Cmd: options:set" << std::endl;
#if 0
            json::value root;
            json::Reader reader;
            if (!reader.parse(data, size, root))          
                throw std::runtime_error("Invalid JSON format: " + reader.getFormattedErrorMessages());
                
            // Do something with JSON data here...
#endif
        }

        // Handle raw file data
        else if (strcmp(node, "file:write") == 0) {
            std::cout << "Launching Cmd: file:write" << std::endl;
        }

        // Handle unknown commands
        else
            throw std::runtime_error("Unknown command");
    }

    // Catch exceptions to set the internal error message and return false
    catch (std::exception& exc)
    {
        _error.assign(exc.what());
        std::cerr << "Command error: " << _error << std::endl;
        return false;
    }

    return true;
}


void OSPlugin::setValue(const char* value)
{
    std::cout << "TestPlugin: Set value: " << value << std::endl;
    _value = value;
}


std::string OSPlugin::sValue() const
{
    return _value;
}


const char* OSPlugin::cValue() const
{
    return _value.c_str();
}


const char* OSPlugin::lastError() const
{
    return _error.empty() ? nullptr : _error.c_str();
}


//
// Static Methods
//


extern "C" SCY_PLUGIN_EXPORT int gimmeFive();


int gimmeFive()
{
    return 5;
}

const char* OSPlugin::GetClassName() const
{
    return "os";
}

std::map<std::string, std::pair<NativeFn, int>> OSPlugin::GetMethods() const
{
    NativeMethods methods =
	{
		std::make_pair<std::string, Native>("which", std::make_pair<NativeFn, int>(whichNative, 0)),
		std::make_pair<std::string, Native>("shell", std::make_pair<NativeFn, int>(shellNative, 1)),
		std::make_pair<std::string, Native>("getenv", std::make_pair<NativeFn, int>(getEnvNative, 1)),
	};
    return methods;
}