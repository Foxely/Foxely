#pragma once

// #define FOXELY_API __attribute__((visibility ("default")))
#include "object.hpp"

extern "C"
{
	static inline Value Fox_StringToValue(const char* str)
	{
		return OBJ_VAL(VM::GetInstance()->m_oParser.TakeString(str));
	}

    static const char* Fox_ValueToCString(Value val)
    {
        return AS_CSTRING(val);
    }
}