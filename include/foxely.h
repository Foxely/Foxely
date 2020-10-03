#pragma once

// #define FOXELY_API __attribute__((visibility ("default")))
#include "object.hpp"

extern "C"
{
#define Fox_RuntimeError(msg, ...) VM::GetInstance()->RuntimeError(msg, ##__VA_ARGS__)

	static inline Value Fox_StringToValue(const char* str)
	{
		return OBJ_VAL(VM::GetInstance()->m_oParser.TakeString(str));
	}

    static const char* Fox_ValueToCString(Value val)
    {
        return AS_CSTRING(val);
    }

    static double Fox_ValueToNumber(Value val)
    {
        return AS_NUMBER(val);
    }

    static inline ObjectAbstract* Fox_ValueToAbstract(Value value)
	{
		return AS_ABSTRACT(value);
	}

    static inline Value Fox_Abstract(void* data, ObjectAbstractType* type)
	{
		return OBJ_VAL(new ObjectAbstract(data, type));
	}

    static inline Value Fox_AbstractToValue(ObjectAbstract* abstract)
	{
		return OBJ_VAL(abstract);
	}

    static inline void* Fox_AbstractGetData(ObjectAbstract* abstract)
	{
		return abstract->data;
	}

    static inline Value Fox_DefineClass(const char* klassName, NativeMethods methods)
	{
        VM::GetInstance()->DefineNativeClass(klassName, methods);
        return NIL_VAL;
	}

    static inline Value Fox_DefineInstanceOf(const char* klassName)
	{
        Value klass;
        Value name = Fox_StringToValue(klassName);
        if (!VM::GetInstance()->globals.Get(AS_STRING(name), klass))
        {
            return NIL_VAL;
        }

		return OBJ_VAL(new ObjectNativeInstance(AS_NATIVE_CLASS(klass)));
	}

    static inline Value Fox_SetInstanceField(Value instance, const char* fieldName, Value value)
	{
        Value name = Fox_StringToValue(fieldName);
        AS_NATIVE_INSTANCE(instance)->fields.Set(AS_STRING(name), value);

		return NIL_VAL;
	}

    static inline Value Fox_GetInstanceField(Value instance, const char* fieldName)
	{
        Value value;
        Value name = Fox_StringToValue(fieldName);
        if (!AS_NATIVE_INSTANCE(instance)->fields.Get(AS_STRING(name), value))
        {
            return NIL_VAL;
        }
		return value;
	}

    static inline void Fox_Arity(int argCount, int min, int max)
	{
        if (argCount < min && argCount > max)
            Fox_RuntimeError("Expected [%d-%d] arguments but got %d.", min, max, argCount);
	}

    static inline void Fox_FixArity(int argCount, int min)
	{
        if (argCount != min)
            Fox_RuntimeError("Expected %d arguments but got %d.", min, argCount);
	}

    static inline bool Fox_Is(Value value, ValueType type)
	{
        return value.type == type;
	}
}