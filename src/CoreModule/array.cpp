#include <iostream>
#include <stdexcept>
#include <string.h>
#include <algorithm>

#include "library/library.h"
#include "value.hpp"
#include "foxely.h"

bool ValidArgs(VM* oVM, int argCount, Value* args)
{
    if (!Fox_Is(args[0], VAL_NUMBER))
    {
        Fox_RuntimeError(oVM, "Expected index number");
        return false;
    }

    int index = AS_INT(args[0]);
    if (index < 0)
    {
        Fox_RuntimeError(oVM, "Expected positive index");
        return false;
    }

    return true;
}

Value getNative(VM* oVM, int argCount, Value* args)
{
    Fox_FixArity(oVM, argCount, 1);
    Fox_PanicIfNot(oVM, !(Fox_Is(args[0], VAL_NUMBER) && Fox_Is(args[0], VAL_INT)), "Expected index number");

    int index = AS_INT(args[0]);
    Fox_PanicIfNot(oVM, index >= 0, "Expected positive index");
    ObjectArray* array = Fox_ValueToArray(args[-1]);
    if (array->m_vValues.empty()) {
        Fox_RuntimeError(oVM, "Cannot access at index %d because the array is empty", index);
    } else if (index >= 0 && index < array->m_vValues.size()) {
        return array->m_vValues[index];
    } else
        Fox_RuntimeError(oVM, "Array index out of bounds.");
    return NIL_VAL;
}

Value setNative(VM* oVM, int argCount, Value* args)
{
    Fox_FixArity(oVM, argCount, 2);
    if (Fox_Is(args[0], VAL_NUMBER))
    {
        int index = AS_NUMBER(args[0]);
        ObjectArray* array = Fox_ValueToArray(args[-1]);
        array->m_vValues[index] = args[1];
    }
    else
        Fox_RuntimeError(oVM, "Expected index number");
    
    return NIL_VAL;
}

Value pushNative(VM* oVM, int argCount, Value* args)
{
    Fox_FixArity(oVM, argCount, 1);
    ObjectArray* array = Fox_ValueToArray(args[-1]);
    array->m_vValues.push_back(args[0]);
    return NIL_VAL;
}

Value popNative(VM* oVM, int argCount, Value* args)
{
    Fox_FixArity(oVM, argCount, 0);
    ObjectArray* array = Fox_ValueToArray(args[-1]);
	Value popped = array->m_vValues.back();
    array->m_vValues.pop_back();
    return popped;
}

Value initNative(VM* oVM, int argCount, Value* args)
{
    Fox_Arity(oVM, argCount, 0, 1);
    Fox_SetInstanceField(oVM, args[-1], "m_oArray", Fox_Array(oVM));

    if (argCount == 1)
    {
        if (Fox_Is(args[0], VAL_NUMBER))
        {
            int size = AS_NUMBER(args[0]);
            ObjectArray* array = Fox_ValueToArray(args[-1]);
            array->m_vValues.reserve(size);
        }
        else
            Fox_RuntimeError(oVM, "Expected number.");
    }
    return NIL_VAL;
}

Value sizeNative(VM* oVM, int argCount, Value* args)
{
    Fox_FixArity(oVM, argCount, 0);
    ObjectArray* array = Fox_ValueToArray(args[-1]);
    return INT_VAL((int) array->m_vValues.size());
}

Value containNative(VM* oVM, int argCount, Value* args)
{
    Fox_FixArity(oVM, argCount, 1);
    ObjectArray* array = Fox_ValueToArray(args[-1]);
    Value search = args[0];
    std::vector<Value>::iterator it = std::find(array->m_vValues.begin(), array->m_vValues.end(), search);

    return BOOL_VAL(it != array->m_vValues.end());
}

Value findNative(VM* oVM, int argCount, Value* args)
{
    Fox_FixArity(oVM, argCount, 1);
    ObjectArray* array = Fox_ValueToArray(args[-1]);
    Value search = args[0];
    std::vector<Value>::iterator it = std::find(array->m_vValues.begin(), array->m_vValues.end(), search);
    int index = distance(array->m_vValues.begin(), it);

    return INT_VAL(index);
}

Value toStringNative(VM* oVM, int argCount, Value* args)
{
    Fox_FixArity(oVM, argCount, 0);
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

    return Fox_StringToValue(oVM, string.c_str());
}

// Value atNative(VM* oVM, int argCount, Value* args)
// {
//     Fox_FixArity(oVM, argCount, 0);
//     Value fpField = Fox_GetInstanceField(args[-1], "m_oFile");

//     if (!ValuesEqual(fpField, NIL_VAL))
//     {
//         FILE* fp = (FILE *) Fox_AbstractGetData(Fox_ValueToAbstract(fpField));
//         fclose(fp);
//     }
    
//     return NIL_VAL;
// }

void DefineCoreArray(VM* oVM)
{
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

    oVM->DefineBuiltIn(oVM->arrayMethods, methods);
}