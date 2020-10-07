#include "library/array.h"
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <algorithm>
#include "foxely.h"
#include "value.hpp"

// SCY_PLUGIN(ArrayPlugin, "IO Module", "0.1.0")

// ObjectAbstractType foxely_file_type =
// {
//     "core/file"
// };

bool ValidArgs(int argCount, Value* args)
{
    if (!Fox_Is(args[0], VAL_NUMBER))
    {
        Fox_RuntimeError("Expected index number");
        return false;
    }

    int index = AS_NUMBER(args[0]);
    if (index < 0)
    {
        Fox_RuntimeError("Expected positive index");
        return false;
    }

    return true;
}

Value getNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 1);
    if (ValidArgs(argCount, args))
    {
        int index = AS_NUMBER(args[0]);
        Value arrayField = Fox_GetInstanceField(args[-1], "m_oArray");
        ObjectArray* array = Fox_ValueToArray(arrayField);
        if (array->m_vValues.empty()) {
            Fox_RuntimeError("Cannot access at index %d because the array is empty", index);
        } else
            return array->m_vValues[index];
    }
    return NIL_VAL;
}

Value setNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 2);
    if (Fox_Is(args[0], VAL_NUMBER))
    {
        int index = AS_NUMBER(args[0]);
        Value arrayField = Fox_GetInstanceField(args[-1], "m_oArray");
        ObjectArray* array = Fox_ValueToArray(arrayField);
        array->m_vValues[index] = args[1];
    }
    else
        Fox_RuntimeError("Expected index number");
    
    return NIL_VAL;
}

Value pushNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 1);
    Value arrayField = Fox_GetInstanceField(args[-1], "m_oArray");
    ObjectArray* array = Fox_ValueToArray(arrayField);
    array->m_vValues.push_back(args[0]);
    return NIL_VAL;
}

Value initNative(int argCount, Value* args)
{
    Fox_Arity(argCount, 0, 1);
    Fox_SetInstanceField(args[-1], "m_oArray", Fox_Array());

    if (argCount == 1)
    {
        if (Fox_Is(args[0], VAL_NUMBER))
        {
            int size = AS_NUMBER(args[0]);
            Value arrayField = Fox_GetInstanceField(args[-1], "m_oArray");
            ObjectArray* array = Fox_ValueToArray(arrayField);
            array->m_vValues.reserve(size);
        }
        else
            Fox_RuntimeError("Expected number.");
    }
    return NIL_VAL;
}

Value sizeNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
    Value arrayField = Fox_GetInstanceField(args[-1], "m_oArray");
    ObjectArray* array = Fox_ValueToArray(arrayField);
    return NUMBER_VAL(array->m_vValues.size());
}

Value containNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 1);
    Value arrayField = Fox_GetInstanceField(args[-1], "m_oArray");
    ObjectArray* array = Fox_ValueToArray(arrayField);
    Value search = args[0];
    std::vector<Value>::iterator it = std::find(array->m_vValues.begin(), array->m_vValues.end(), search);

    return BOOL_VAL(it != array->m_vValues.end());
}

Value findNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 1);
    Value arrayField = Fox_GetInstanceField(args[-1], "m_oArray");
    ObjectArray* array = Fox_ValueToArray(arrayField);
    Value search = args[0];
    std::vector<Value>::iterator it = std::find(array->m_vValues.begin(), array->m_vValues.end(), search);
    int index = distance(array->m_vValues.begin(), it);

    return NUMBER_VAL(index);
}

Value toStringNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
    Value arrayField = Fox_GetInstanceField(args[-1], "m_oArray");
    ObjectArray* array = Fox_ValueToArray(arrayField);

    std::string string;

    string += "[";
    int size = array->m_vValues.size();
    for (auto& it : array->m_vValues)
    {
        size--;
        string += ValueToString(it);
        if (size > 0)
            string += ", ";
    }
    string += "]";

    return Fox_StringToValue(string.c_str());
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

	NativeMethods methods =
	{
		std::make_pair<std::string, NativeFn>("init", initNative),
		std::make_pair<std::string, NativeFn>("push", pushNative),
		std::make_pair<std::string, NativeFn>("get", getNative),
		std::make_pair<std::string, NativeFn>("set", setNative),
		std::make_pair<std::string, NativeFn>("size", sizeNative),
		std::make_pair<std::string, NativeFn>("contain", containNative),
		std::make_pair<std::string, NativeFn>("find", findNative),
		std::make_pair<std::string, NativeFn>("toString", toStringNative),
	};

	VM::GetInstance()->DefineNativeClass("Array", methods);
	m_oMethods = methods;
}

ArrayPlugin::~ArrayPlugin()
{
    // std::cout << "ArrayPlugin: Destroy" << std::endl;
}

const char* ArrayPlugin::GetClassName() const
{
    return "Array";
}

FOX_MODULE(array)
{
	NativeMethods methods =
	{
		std::make_pair<std::string, NativeFn>("init", initNative),
		std::make_pair<std::string, NativeFn>("push", pushNative),
		std::make_pair<std::string, NativeFn>("get", getNative),
		std::make_pair<std::string, NativeFn>("set", setNative),
		std::make_pair<std::string, NativeFn>("size", sizeNative),
		std::make_pair<std::string, NativeFn>("contain", containNative),
		std::make_pair<std::string, NativeFn>("find", findNative),
		std::make_pair<std::string, NativeFn>("toString", toStringNative),
	};

	VM::GetInstance()->DefineNativeClass("Array", methods);
}