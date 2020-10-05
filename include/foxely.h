#pragma once

// #define FOXELY_API __attribute__((visibility ("default")))
#include "Parser.h"
#include "vm.hpp"
#include "object.hpp"

extern "C"
{
#define Fox_RuntimeError(msg, ...) VM::GetInstance()->RuntimeError(msg, ##__VA_ARGS__)
#define Fox_PanicIfNot(cond, msg, ...) if (!(cond)) { Fox_RuntimeError(msg, ##__VA_ARGS__); return NIL_VAL; }

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

    static inline ObjectArray* Fox_ValueToArray(Value value)
	{
		return AS_ARRAY(value);
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
            return NIL_VAL;

		return OBJ_VAL(new ObjectNativeInstance(AS_NATIVE_CLASS(klass)));
	}

    static inline void Fox_CallMethod(Value instance, const char* methodName, int argCount, Value* params)
	{
        Value method;
        Value methodNameValue = Fox_StringToValue(methodName);
        if (AS_NATIVE_INSTANCE(instance)->klass->methods.Get(AS_STRING(methodNameValue), method))
        {
            AS_NATIVE(method)(argCount, params);
        }
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

    static inline bool Fox_IsObject(Value value)
	{
        return IS_OBJ(value);
	}

    static inline bool Fox_IsString(Value value)
	{
        return IS_STRING(value);
	}

    static inline bool Fox_IsNativeInstance(Value value)
	{
        return IS_NATIVE_INSTANCE(value);
	}

    static inline bool Fox_IsNumber(Value value)
	{
        return IS_NUMBER(value);
	}

    static inline Value Fox_Array()
	{
		return OBJ_VAL(new ObjectArray());
	}
}