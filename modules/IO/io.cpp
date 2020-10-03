#include "io.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <fstream>
#include "foxely.h"

SCY_PLUGIN(IOPlugin, "IO Module", "0.1.0")

ObjectAbstractType foxely_file_type =
{
    "core/file"
};

Value openNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 1);
    FILE* fp = fopen(Fox_ValueToCString(args[0]), "r");
    if (fp)
    {
        Value instance = Fox_DefineInstanceOf("File");
        Fox_SetInstanceField(instance, "m_oFile", Fox_Abstract(fp, &foxely_file_type));
        return instance;
    }
    return NIL_VAL;
}

Value readNative(int argCount, Value* args)
{
    Fox_Arity(argCount, 0, 1);
    Value fpField = Fox_GetInstanceField(args[-1], "m_oFile");

    FILE* fp = (FILE *) Fox_AbstractGetData(Fox_ValueToAbstract(fpField));

    if (argCount == 0)
    {
        size_t len = 0;

        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        rewind(fp);

        char fcontent[len];
        fread(fcontent, 1, len, fp);

        fcontent[len] = 0;

        return Fox_StringToValue(fcontent);
    }
    else
    {
        if (Fox_Is(args[0], VAL_NUMBER))
        {
            int size = Fox_ValueToNumber(args[0]);
            char chunk[size];

            if (fgets(chunk, sizeof(chunk), fp) != NULL)
                return Fox_StringToValue(chunk);
        }
        else
            Fox_RuntimeError("Invalid type, expected number type");
        return NIL_VAL;
    }
    return NIL_VAL;
}

Value readLineNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
    Value fpField = Fox_GetInstanceField(args[-1], "m_oFile");
    FILE* fp = (FILE *) Fox_AbstractGetData(Fox_ValueToAbstract(fpField));
    size_t len = 0;
    char* line = NULL;
    ssize_t read;
    
    read = getline(&line, &len, fp);
    if (read != -1) {
        Value lineValue = Fox_StringToValue(line);
        if (line)
            free(line);
        return lineValue;
    }
    return NIL_VAL;
}

Value closeNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
    Value fpField = Fox_GetInstanceField(args[-1], "m_oFile");

    if (!ValuesEqual(fpField, NIL_VAL))
    {
        FILE* fp = (FILE *) Fox_AbstractGetData(Fox_ValueToAbstract(fpField));
        fclose(fp);
    }
    
    return NIL_VAL;
}



IOPlugin::IOPlugin()
{
    // std::cout << "IOPlugin: Create" << std::endl;
}

IOPlugin::~IOPlugin()
{
    // std::cout << "IOPlugin: Destroy" << std::endl;
}

const char* IOPlugin::GetClassName() const
{
    return "io";
}

NativeMethods IOPlugin::GetMethods() const
{
	NativeMethods methods =
	{
		std::make_pair<std::string, NativeFn>("open", openNative),
	};

    NativeMethods fileMethods =
	{
		std::make_pair<std::string, NativeFn>("read", readNative),
		std::make_pair<std::string, NativeFn>("readline", readLineNative),
		std::make_pair<std::string, NativeFn>("close", closeNative),
	};

    Fox_DefineClass("File", fileMethods);

	return methods;
}