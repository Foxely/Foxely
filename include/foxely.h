#pragma once

// #include <string>
// #include "Parser.h"
// #include "vm.hpp"
// #define FOXELY_API __attribute__((visibility ("default")))
#include "object.hpp"

// class Value;

extern "C"
{
    // FOXELY_API ObjectString* TakeCString(const char* value);
    // FOXELY_API Value Fox_StringToValue(VM* vm, const char* str);

    static const char* Fox_ValueToCString(Value val)
    {
        return AS_CSTRING(val);
    }
}
// #ifdef __cpluscplus
// 	class Value;
// 	class VM;
// 	extern "C" {
// #else
// 	struct Value;
// 	struct VM;
// 	typedef struct VM VM;
// #endif

// 	Value Fox_StringToValue(VM* vm, const char* str);

// 	static const char* Fox_ValueToCString(Value val)
// 	{
// 		return AS_CSTRING(val);
// 	}

// #ifdef __cpluscplus
// 	}
// #endif