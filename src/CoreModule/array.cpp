#include <iostream>
#include <stdexcept>
#include <string.h>
#include <algorithm>

#include "library/library.h"
#include "value.hpp"
#include "foxely.h"

bool ValidArgs(VM* pVM, int argCount, Value* args)
{
    if (!Fox_IsInt(args[0]))
    {
        Fox_RuntimeError(pVM, "Expected index number");
        return false;
    }

    int index = Fox_AsInt(args[0]);
    if (index < 0)
    {
        Fox_RuntimeError(pVM, "Expected positive index");
        return false;
    }

    return true;
}

Value getNative(VM* pVM, int argCount, Value* args)
{
    Fox_FixArity(pVM, argCount, 1);
    Fox_PanicIfNot(pVM, Fox_IsInt(args[0]), "Expected index number");

    int index = Fox_AsInt(args[0]);
    Fox_PanicIfNot(pVM, index >= 0, "Expected positive index");
    ref<ObjectArray> array = Fox_AsArray(args[-1]);
    if (array->m_vValues.empty()) {
        Fox_RuntimeError(pVM, "Cannot access at index %d because the array is empty", index);
    } else if (index >= 0 && index < array->m_vValues.size()) {
        return array->m_vValues[index];
    } else
        Fox_RuntimeError(pVM, "Array index out of bounds.");
    return Fox_Nil;
}

Value setNative(VM* pVM, int argCount, Value* args)
{
    Fox_FixArity(pVM, argCount, 2);
    if (Fox_IsInt(args[0]))
    {
        int index = Fox_IsInt(args[0]);
        ref<ObjectArray> array = Fox_AsArray(args[-1]);
        array->m_vValues[index] = args[1];
    }
    else
        Fox_RuntimeError(pVM, "Expected index number");
    
    return Fox_Nil;
}

Value pushNative(VM* pVM, int argCount, Value* args)
{
    Fox_FixArity(pVM, argCount, 1);
    ref<ObjectArray> array = Fox_AsArray(args[-1]);
    array->m_vValues.push_back(args[0]);
    return Fox_Nil;
}

Value popNative(VM* pVM, int argCount, Value* args)
{
    Fox_FixArity(pVM, argCount, 0);
    ref<ObjectArray> array = Fox_AsArray(args[-1]);
	Value popped = array->m_vValues.back();
    array->m_vValues.pop_back();
    return popped;
}

Value sizeNative(VM* pVM, int argCount, Value* args)
{
    Fox_FixArity(pVM, argCount, 0);
    ref<ObjectArray> array = Fox_AsArray(args[-1]);
    return Fox_Int((int) array->m_vValues.size());
}

Value containNative(VM* pVM, int argCount, Value* args)
{
    Fox_FixArity(pVM, argCount, 1);
    ref<ObjectArray> array = Fox_AsArray(args[-1]);
    Value search = args[0];
    std::vector<Value>::iterator it = std::find(array->m_vValues.begin(), array->m_vValues.end(), search);

    return Fox_Bool(it != array->m_vValues.end());
}

Value findNative(VM* pVM, int argCount, Value* args)
{
    Fox_FixArity(pVM, argCount, 1);
    ref<ObjectArray> array = Fox_AsArray(args[-1]);
    Value search = args[0];
    std::vector<Value>::iterator it = std::find(array->m_vValues.begin(), array->m_vValues.end(), search);
    int index = distance(array->m_vValues.begin(), it);

    return Fox_Int(index);
}

Value toStringNative(VM* pVM, int argCount, Value* args)
{
    Fox_FixArity(pVM, argCount, 0);
    ref<ObjectArray> pArray = Fox_AsArray(args[-1]);

    std::string string;

    // string += "[";
    int size = pArray->m_vValues.size();
    for (auto& it : pArray->m_vValues)
    {
        size--;
        string += ValueToString(it, pVM);
        if (size > 0)
            string += ",";
    }
    // string += "]";

    return Fox_NewString(pVM, string.c_str());
}

// Value atNative(VM* pVM, int argCount, Value* args)
// {
//     Fox_FixArity(pVM, argCount, 0);
//     Value fpField = Fox_GetInstanceField(args[-1], "m_oFile");

//     if (!ValuesEqual(fpField, Fox_Nil))
//     {
//         FILE* fp = (FILE *) Fox_AbstractGetData(Fox_ValueToAbstract(fpField));
//         fclose(fp);
//     }
    
//     return Fox_Nil;
// }

void DefineCoreArray(VM* pVM)
{
    NativeMethods oMethods =
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

    pVM->DefineBuiltIn(pVM->arrayMethods, oMethods);
}