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

#define Fox_ApiError(pVM, msg, ...) { fprintf(stderr, msg, ##__VA_ARGS__); pVM->result = INTERPRET_RUNTIME_ERROR; }
#define Fox_RuntimeError(pVM, msg, ...) pVM->RuntimeError(msg, ##__VA_ARGS__)
#define Fox_PanicIfNot(pVM, cond, msg, ...) if (!(cond)) { Fox_RuntimeError(pVM, msg, ##__VA_ARGS__); return NIL_VAL; }
#define Fox_ApiPanicIfNot(pVM, cond, msg, ...) if (!(cond)) { Fox_ApiError(pVM, msg, ##__VA_ARGS__); exit(84); }

	static inline Value Fox_String(VM* pVM, const char* str)
	{
		return OBJ_VAL(pVM->m_oParser.TakeString(str));
	}

	static inline Value Fox_NumberToValue(double dNumber)
	{
		return NUMBER_VAL(dNumber);
	}

	static inline Value Fox_IntegerToValue(int iNumber)
	{
		return INT_VAL(iNumber);
	}

	static inline Value Fox_BoolToValue(bool bValue)
	{
		return BOOL_VAL(bValue);
	}

	static inline Value Fox_ObjectToValue(Object* pObject)
	{
		return OBJ_VAL(pObject);
	}

    static const char* Fox_ValueToCString(Value val)
    {
        return AS_CSTRING(val);
    }

    static double Fox_ValueToNumber(Value val)
    {
        return AS_NUMBER(val);
    }

	static int Fox_ValueToInteger(Value val)
    {
        return AS_INT(val);
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

	static inline ObjectInstance* Fox_ValueToInstance(Value value)
	{
		return AS_INSTANCE(value);
	}

    static inline Value Fox_Abstract(VM* pVM, void* data, ObjectAbstractType* type)
	{
		return Fox_ObjectToValue(pVM->gc.New<ObjectAbstract>(data, type));
	}

    static inline Value Fox_AbstractToValue(ObjectAbstract* abstract)
	{
		return Fox_ObjectToValue(abstract);
	}

    static inline void* Fox_AbstractGetData(ObjectAbstract* abstract)
	{
		return abstract->data;
	}

    static inline Value Fox_DefineClass(VM* pVM, const char* strModuleName, const char* strClassName, NativeMethods methods)
	{
        pVM->DefineClass(strModuleName, strClassName, methods);
        return NIL_VAL;
	}

    static inline Value Fox_DefineInstanceOf(VM* pVM, const char* strModuleName, const char* strKlassName)
	{
		// See if the module has already been loaded.
		ObjectModule* pModule = pVM->GetModule(Fox_String(pVM, strModuleName));
		FOX_ASSERT(pModule != NULL, "Module not found.");

		if (pModule != NULL)
		{
        	Value oKlass;
			if (pModule->m_vVariables.Get(AS_STRING(Fox_String(pVM, strKlassName)), oKlass))
			{
				return OBJ_VAL(pVM->gc.New<ObjectInstance>(AS_CLASS(oKlass)));
			}
		}
        return NIL_VAL;
	}

	static inline Value Fox_DefineInstanceOfCStruct(VM* pVM, const char* strModuleName, const char* strKlassName, void* cStruct)
	{
        // See if the module has already been loaded.
		ObjectModule* pModule = pVM->GetModule(Fox_String(pVM, strModuleName));
		if (pModule != NULL)
		{
        	Value oKlass;
			if (pModule->m_vVariables.Get(AS_STRING(Fox_String(pVM, strKlassName)), oKlass))
			{
				ObjectInstance* pInstance = pVM->gc.New<ObjectInstance>(AS_CLASS(oKlass), cStruct);
				pVM->Push(OBJ_VAL(pInstance));
				Value oInitializer;
				if (AS_CLASS(oKlass)->methods.Get(pVM->initString, oInitializer))
					pVM->CallValue(oInitializer, 0);
				pVM->Pop();
				return OBJ_VAL(pInstance);
			}
		}
        return NIL_VAL;
	}

	static inline Value Fox_Setter(VM* pVM, ObjectInstance* pInstance, const char* strFieldName, NativeFn oSetter)
	{
        // See if the module has already been loaded.
		if (pInstance != NULL)
		{
        	pInstance->setters.Set(AS_STRING(Fox_String(pVM, strFieldName)), Fox_ObjectToValue(pVM->gc.New<ObjectNative>(oSetter)));
		}
        return NIL_VAL;
	}

	static inline Value Fox_Getter(VM* pVM, ObjectInstance* pInstance, const char* strFieldName, NativeFn oSetter)
	{
        // See if the module has already been loaded.
		if (pInstance != NULL)
		{
        	pInstance->getters.Set(AS_STRING(Fox_String(pVM, strFieldName)), Fox_ObjectToValue(pVM->gc.New<ObjectNative>(oSetter)));
		}
        return NIL_VAL;
	}

    static inline Value Fox_SetField(VM* pVM, Value oInstance, const char* fieldName, Value value)
	{
        Value name = Fox_String(pVM, fieldName);
        AS_INSTANCE(oInstance)->fields.Set(AS_STRING(name), value);

		return NIL_VAL;
	}

    static inline Value Fox_GetField(VM* pVM, Value oInstance, const char* fieldName)
	{
        Value value;
        Value name = Fox_String(pVM, fieldName);
        if (!AS_INSTANCE(oInstance)->fields.Get(AS_STRING(name), value))
        {
            return NIL_VAL;
        }
		return value;
	}

	static inline void* Fox_GetUserData(Value oInstance)
	{
        return AS_INSTANCE(oInstance)->cStruct;
	}

	static inline void* Fox_SetUserData(Value oInstance, void* data)
	{
		AS_INSTANCE(oInstance)->cStruct = data;
        return AS_INSTANCE(oInstance)->cStruct;
	}

    static inline void Fox_Arity(VM* pVM, int argCount, int min, int max)
	{
        if (argCount < min && argCount > max)
            Fox_RuntimeError(pVM, "Expected [%d-%d] arguments but got %d.", min, max, argCount);
	}

    static inline void Fox_FixArity(VM* pVM, int argCount, int min)
	{
        if (argCount != min)
            Fox_RuntimeError(pVM, "Expected %d arguments but got %d.", min, argCount);
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

    static inline bool Fox_IsNumber(Value value)
	{
        return IS_NUMBER(value);
	}

	static inline bool Fox_IsBool(Value value)
	{
        return IS_BOOL(value);
	}

    static inline Value Fox_Array(VM* pVM)
	{
		return OBJ_VAL(pVM->gc.New<ObjectArray>());
	}
}