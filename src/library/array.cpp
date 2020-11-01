#include "library/array.h"
#include <iostream>
#include <stdexcept>
#include <string.h>
#include <algorithm>
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

    int index = AS_INT(args[0]);
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
    Fox_PanicIfNot(!(Fox_Is(args[0], VAL_NUMBER) && Fox_Is(args[0], VAL_INT)), "Expected index number");

    int index = AS_INT(args[0]);
    Fox_PanicIfNot(index >= 0, "Expected positive index");
    ObjectArray* array = Fox_ValueToArray(args[-1]);
    if (array->m_vValues.empty()) {
        Fox_RuntimeError("Cannot access at index %d because the array is empty", index);
    } else if (index >= 0 && index < array->m_vValues.size()) {
        return array->m_vValues[index];
    } else
        Fox_RuntimeError("Array index out of bounds.");
    return NIL_VAL;
}

Value setNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 2);
    if (Fox_Is(args[0], VAL_NUMBER))
    {
        int index = AS_NUMBER(args[0]);
        ObjectArray* array = Fox_ValueToArray(args[-1]);
        array->m_vValues[index] = args[1];
    }
    else
        Fox_RuntimeError("Expected index number");
    
    return NIL_VAL;
}

Value pushNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 1);
    ObjectArray* array = Fox_ValueToArray(args[-1]);
    array->m_vValues.push_back(args[0]);
    return NIL_VAL;
}

Value popNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
    ObjectArray* array = Fox_ValueToArray(args[-1]);
	Value popped = array->m_vValues.back();
    array->m_vValues.pop_back();
    return popped;
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
            ObjectArray* array = Fox_ValueToArray(args[-1]);
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
    ObjectArray* array = Fox_ValueToArray(args[-1]);
    return INT_VAL((int) array->m_vValues.size());
}

Value containNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 1);
    ObjectArray* array = Fox_ValueToArray(args[-1]);
    Value search = args[0];
    std::vector<Value>::iterator it = std::find(array->m_vValues.begin(), array->m_vValues.end(), search);

    return BOOL_VAL(it != array->m_vValues.end());
}

Value findNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 1);
    ObjectArray* array = Fox_ValueToArray(args[-1]);
    Value search = args[0];
    std::vector<Value>::iterator it = std::find(array->m_vValues.begin(), array->m_vValues.end(), search);
    int index = distance(array->m_vValues.begin(), it);

    return INT_VAL(index);
}

Value toStringNative(int argCount, Value* args)
{
    Fox_FixArity(argCount, 0);
    ObjectArray* array = Fox_ValueToArray(args[-1]);

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
		std::make_pair<std::string, NativeFn>("push", pushNative),
		std::make_pair<std::string, NativeFn>("pop", popNative),
		std::make_pair<std::string, NativeFn>("get", getNative),
		std::make_pair<std::string, NativeFn>("set", setNative),
		std::make_pair<std::string, NativeFn>("size", sizeNative),
		std::make_pair<std::string, NativeFn>("contain", containNative),
		std::make_pair<std::string, NativeFn>("find", findNative),
		std::make_pair<std::string, NativeFn>("toString", toStringNative),
	};

	VM::GetInstance()->DefineBuiltIn(VM::GetInstance()->arrayMethods, methods);
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
		std::make_pair<std::string, NativeFn>("push", pushNative),
		std::make_pair<std::string, NativeFn>("get", getNative),
		std::make_pair<std::string, NativeFn>("set", setNative),
		std::make_pair<std::string, NativeFn>("size", sizeNative),
		std::make_pair<std::string, NativeFn>("contain", containNative),
		std::make_pair<std::string, NativeFn>("find", findNative),
		std::make_pair<std::string, NativeFn>("toString", toStringNative),
	};
	VM::GetInstance()->DefineBuiltIn(VM::GetInstance()->arrayMethods, methods);
}
