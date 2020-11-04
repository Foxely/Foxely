#pragma once

// #define FOXELY_API __attribute__((visibility ("default")))
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "linenoise.hpp"
#include "Parser.h"
#include "object.hpp"
#include "vm.hpp"

extern "C"
{
#define FOX_MODULE(name) void name##_entry()
#define FOX_MODULE_CALL(name) name##_entry()

#define Fox_RuntimeError(oVM, msg, ...) oVM->RuntimeError(msg, ##__VA_ARGS__)
#define Fox_PanicIfNot(oVM, cond, msg, ...) if (!(cond)) { Fox_RuntimeError(oVM, msg, ##__VA_ARGS__); return NIL_VAL; }

	static inline Value Fox_StringToValue(VM* oVM, const char* str)
	{
		return OBJ_VAL(oVM->m_oParser.TakeString(str));
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

	static inline bool Fox_ValueToBool(Value value)
	{
		return AS_BOOL(value);
	}


    static inline Value Fox_Abstract(VM* oVM, void* data, ObjectAbstractType* type)
	{
		return OBJ_VAL(oVM->gc.New<ObjectAbstract>(data, type));
	}

    static inline Value Fox_AbstractToValue(ObjectAbstract* abstract)
	{
		return OBJ_VAL(abstract);
	}

    static inline void* Fox_AbstractGetData(ObjectAbstract* abstract)
	{
		return abstract->data;
	}

    static inline Value Fox_DefineClass(VM* oVM, const char* strModuleName, const char* strClassName, NativeMethods methods)
	{
        oVM->DefineClass(strModuleName, strClassName, methods);
        return NIL_VAL;
	}

    static inline Value Fox_DefineInstanceOf(VM* oVM, const char* klassName)
	{
        Value klass;
        Value name = Fox_StringToValue(oVM, klassName);
        if (!oVM->globals.Get(AS_STRING(name), klass))
            return NIL_VAL;

		return OBJ_VAL(oVM->gc.New<ObjectNativeInstance>(AS_NATIVE_CLASS(klass)));
	}

	static inline Value Fox_DefineInstanceOfCStruct(VM* oVM, const char* klassName, void* cStruct)
	{
        Value klass;
        Value name = Fox_StringToValue(oVM, klassName);
        if (!oVM->globals.Get(AS_STRING(name), klass))
            return NIL_VAL;

		return OBJ_VAL(oVM->gc.New<ObjectNativeInstance>(AS_NATIVE_CLASS(klass), cStruct));
	}

    static inline void Fox_CallMethod(VM* oVM, Value instance, const char* methodName, int argCount, Value* params)
	{
        Value method;
        Value methodNameValue = Fox_StringToValue(oVM, methodName);
        if (AS_NATIVE_INSTANCE(instance)->klass->methods.Get(AS_STRING(methodNameValue), method))
        {
            AS_NATIVE(method)(oVM, argCount, params);
        }
	}

    static inline Value Fox_SetInstanceField(VM* oVM, Value instance, const char* fieldName, Value value)
	{
        Value name = Fox_StringToValue(oVM, fieldName);
        AS_NATIVE_INSTANCE(instance)->fields.Set(AS_STRING(name), value);

		return NIL_VAL;
	}

    static inline Value Fox_GetInstanceField(VM* oVM, Value instance, const char* fieldName)
	{
        Value value;
        Value name = Fox_StringToValue(oVM, fieldName);
        if (!AS_NATIVE_INSTANCE(instance)->fields.Get(AS_STRING(name), value))
        {
            return NIL_VAL;
        }
		return value;
	}

	static inline void* Fox_GetInstanceCStruct(Value instance)
	{
        return AS_NATIVE_INSTANCE(instance)->cStruct;
	}

	static inline void* Fox_SetInstanceCStruct(Value instance, void* data)
	{
		AS_NATIVE_INSTANCE(instance)->cStruct = data;
        return AS_NATIVE_INSTANCE(instance)->cStruct;
	}

    static inline void Fox_Arity(VM* oVM, int argCount, int min, int max)
	{
        if (argCount < min && argCount > max)
            Fox_RuntimeError(oVM, "Expected [%d-%d] arguments but got %d.", min, max, argCount);
	}

    static inline void Fox_FixArity(VM* oVM, int argCount, int min)
	{
        if (argCount != min)
            Fox_RuntimeError(oVM, "Expected %d arguments but got %d.", min, argCount);
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

	static inline bool Fox_IsBool(Value value)
	{
        return IS_BOOL(value);
	}

    static inline Value Fox_Array(VM* oVM)
	{
		return OBJ_VAL(oVM->gc.New<ObjectArray>());
	}
}