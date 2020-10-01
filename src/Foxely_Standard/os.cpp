
#include <stdlib.h>
#include "foxely.h"

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