#include <iostream>
#include <stdexcept>
#include <string.h>
#include <algorithm>

#include "library/library.h"
#include "value.hpp"
#include "foxely.h"

Value getMapNative(VM* pVM, int argCount, Value* args)
{
    Fox_FixArity(pVM, argCount, 1);

    Value oValue;
    ObjectMap* pMap = Fox_AsMap(args[-1]);
    
    if (pMap->m_vValues.Get(args[-1], oValue))
        return oValue;
    return Fox_Nil;
}

Value setMapNative(VM* pVM, int argCount, Value* args)
{
    Fox_FixArity(pVM, argCount, 2);
    ObjectMap* pMap = Fox_AsMap(args[-1]);
    pMap->m_vValues.Set(args[0], args[1]);

    return Fox_Nil;
}

Value pushMapNative(VM* pVM, int argCount, Value* args)
{
    Fox_FixArity(pVM, argCount, 2);
    ObjectMap* pMap = Fox_AsMap(args[-1]);
    pMap->m_vValues.Set(args[0], args[1]);
    return Fox_Nil;
}

Value popMapNative(VM* pVM, int argCount, Value* args)
{
    Fox_FixArity(pVM, argCount, 0);

    ObjectMap* pMap1 = Fox_AsMap(args[-1]);

    MapEntry oEntry = pMap1->m_vValues.m_vEntries.back();

    while (Fox_IsNil(oEntry.m_oKey) && Fox_IsNil(oEntry.m_oValue))
    {
        pMap1->m_vValues.m_vEntries.pop_back();
        oEntry = pMap1->m_vValues.m_vEntries.back();
    }
    pMap1->m_vValues.Delete(oEntry.m_oKey);
    // pMap1->m_vValues.m_iCapacity = pMap1->m_vValues.m_vEntries.size();

    ObjectMap* pMap2 = pVM->gc.New<ObjectMap>();
    pMap2->m_vValues.Set(oEntry.m_oKey, oEntry.m_oValue);

    return Fox_Object(pMap2);
}

Value sizeMapNative(VM* pVM, int argCount, Value* args)
{
    Fox_FixArity(pVM, argCount, 0);
    ObjectMap* pMap = Fox_AsMap(args[-1]);
    return Fox_Int(pMap->m_vValues.Size());
}

Value containMapNative(VM* pVM, int argCount, Value* args)
{
    Fox_FixArity(pVM, argCount, 1);
    ObjectMap* pMap = Fox_AsMap(args[-1]);
    Value search = args[0];
    MapEntry it = pMap->m_vValues.FindEntry(search);

    return Fox_Bool(!(it.m_oKey == Fox_Nil) && !(it.m_oValue == Fox_Nil));
}

Value toStringMapNative(VM* pVM, int argCount, Value* args)
{
    Fox_FixArity(pVM, argCount, 0);
    ObjectMap* pMap = Fox_AsMap(args[-1]);

    std::string string;

    string += "{";
    int size = pMap->m_vValues.Size();
    for (auto& it : pMap->m_vValues.m_vEntries)
    {
        if (!(it.m_oKey == Fox_Nil) && !(it.m_oValue == Fox_Nil))
        {
            size--;
            string += ValueToString(it.m_oKey);
            string += ": ";
            string += ValueToString(it.m_oValue);
            if (size > 0)
                string += ", ";
        }
    }
    string += "}";

    return Fox_NewString(pVM, string.c_str());
}

void DefineCoreMap(VM* pVM)
{
    NativeMethods oMethods =
	{
		std::make_pair<std::string, NativeFn>("push", pushMapNative),
		std::make_pair<std::string, NativeFn>("pop", popMapNative),
		std::make_pair<std::string, NativeFn>("remove", popMapNative),
		std::make_pair<std::string, NativeFn>("get", getMapNative),
		std::make_pair<std::string, NativeFn>("set", setMapNative),
		std::make_pair<std::string, NativeFn>("size", sizeMapNative),
		std::make_pair<std::string, NativeFn>("contain", containMapNative),
		std::make_pair<std::string, NativeFn>("toString", toStringMapNative),
	};

    pVM->DefineBuiltIn(pVM->mapMethods, oMethods);
}