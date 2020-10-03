#include "library/array.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <fstream>
#include "foxely.h"

// SCY_PLUGIN(ArrayPlugin, "IO Module", "0.1.0")

// ObjectAbstractType foxely_file_type =
// {
//     "core/file"
// };

Value getNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 1);
    return NIL_VAL;
}

Value setNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 2);
    return NIL_VAL;
}

Value pushNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 1);
    return NIL_VAL;
}

Value initNative(int argCount, Value* args)
{
    return NIL_VAL;
}

// Value atNative(int argCount, Value* args)
// {
//     Fox_FixArity(argCount, 0);
//     Value fpField = Fox_GetInstanceField(args[-1], "m_oFile");

//     if (!ValuesEqual(fpField, NIL_VAL))
//     {
//         FILE* fp = (FILE *) Fox_AbstractGetData(Fox_ValueToAbstract(fpField));
//         fclose(fp);
//     }
    
//     return NIL_VAL;
// }



ArrayPlugin::ArrayPlugin()
{
    // std::cout << "ArrayPlugin: Create" << std::endl;
}

ArrayPlugin::~ArrayPlugin()
{
    // std::cout << "ArrayPlugin: Destroy" << std::endl;
}

const char* ArrayPlugin::GetClassName() const
{
    return "array";
}

NativeMethods ArrayPlugin::GetMethods()
{
	NativeMethods methods =
	{
		std::make_pair<std::string, NativeFn>("init", initNative),
		std::make_pair<std::string, NativeFn>("push", pushNative),
		std::make_pair<std::string, NativeFn>("get", getNative),
		std::make_pair<std::string, NativeFn>("set", setNative),
	};

	return methods;
}