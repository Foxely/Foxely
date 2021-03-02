
// #define FOXELY_API __attribute__((visibility ("default")))

#ifndef FOX_FOXELY_HPP_
#define FOX_FOXELY_HPP_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "Parser.h"
#include "object.hpp"
#include "vm.hpp"

extern "C"
{
#define FOX_MODULE(name) void name##_entry()
#define FOX_MODULE_CALL(name) name##_entry()

#define Fox_RuntimeError(pVM, msg, ...) pVM->RuntimeError(msg, ##__VA_ARGS__)
#define Fox_PanicIfNot(pVM, cond, msg, ...) if (!(cond)) { Fox_RuntimeError(pVM, msg, ##__VA_ARGS__); return Fox_Nil; }
#define Fox_ApiPanicIfNot(pVM, cond, msg, ...) if (!(cond)) { Fox_ApiError(pVM, msg, ##__VA_ARGS__); exit(84); }

#define Fox_Arity(pVM, argCount, min, max) \
        if (argCount < min && argCount > max) { \
            Fox_RuntimeError(pVM, "Expected [%d-%d] arguments but got %d.", min, max, argCount); \
            return Fox_Nil; \
        }

#define Fox_FixArity(pVM, argCount, min) \
        if (argCount != min) { \
            Fox_RuntimeError(pVM, "Expected %d arguments but got %d.", min, argCount); \
            return Fox_Nil; \
        }

	static inline Value Fox_NewString(VM* pVM, const char* str)
	{
		return Fox_Object(pVM->m_oParser.TakeString(str));
	}

    static inline Value Fox_Abstract(VM* pVM, void* data, ObjectAbstractType* type)
	{
		return Fox_Object(pVM->gc.New<ObjectAbstract>(data, type));
	}

    static inline Value Fox_AbstractToValue(ObjectAbstract* abstract)
	{
		return Fox_Object(abstract);
	}

    static inline void* Fox_AbstractGetData(ObjectAbstract* abstract)
	{
		return abstract->data;
	}

    static inline Value Fox_DefineClass(VM* pVM, const char* strModuleName, const char* strClassName, NativeMethods methods)
	{
        pVM->DefineClass(strModuleName, strClassName, methods);
        return Fox_Nil;
	}

    static inline Value Fox_DefineInstanceOf(VM* pVM, const char* strModuleName, const char* strKlassName)
	{
		// See if the module has already been loaded.
		ObjectModule* pModule = pVM->GetModule(Fox_NewString(pVM, strModuleName));
		FOX_ASSERT(pModule != NULL, "Module not found.");

		if (pModule != NULL)
		{
        	Value oKlass;
			if (pModule->m_vVariables.Get(Fox_AsString(Fox_NewString(pVM, strKlassName)), oKlass))
			{
				return Fox_Object(pVM->gc.New<ObjectInstance>(Fox_AsClass(oKlass)));
			}
		}
        return Fox_Nil;
	}

	static inline Value Fox_DefineInstanceOfCStruct(VM* pVM, const char* strModuleName, const char* strKlassName, void* cStruct)
	{
        // See if the module has already been loaded.
		ObjectModule* pModule = pVM->GetModule(Fox_NewString(pVM, strModuleName));
		if (pModule != NULL)
		{
        	Value oKlass;
			if (pModule->m_vVariables.Get(Fox_AsString(Fox_NewString(pVM, strKlassName)), oKlass))
			{
				ObjectInstance* pInstance = pVM->gc.New<ObjectInstance>(Fox_AsClass(oKlass), cStruct);
				pVM->Push(Fox_Object(pInstance));
				Value oInitializer;
				if (Fox_AsClass(oKlass)->methods.Get(pVM->initString, oInitializer))
					pVM->CallValue(oInitializer, 0);
				pVM->Pop();
				return Fox_Object(pInstance);
			}
		}
        return Fox_Nil;
	}

	static inline Value Fox_Setter(VM* pVM, ObjectInstance* pInstance, const char* strFieldName, NativeFn oSetter)
	{
        // See if the module has already been loaded.
		if (pInstance != NULL)
		{
        	pInstance->setters.Set(Fox_AsString(Fox_NewString(pVM, strFieldName)), Fox_Object(pVM->gc.New<ObjectNative>(oSetter)));
		}
        return Fox_Nil;
	}

	static inline Value Fox_Getter(VM* pVM, ObjectInstance* pInstance, const char* strFieldName, NativeFn oSetter)
	{
        // See if the module has already been loaded.
		if (pInstance != NULL)
		{
        	pInstance->getters.Set(Fox_AsString(Fox_NewString(pVM, strFieldName)), Fox_Object(pVM->gc.New<ObjectNative>(oSetter)));
		}
        return Fox_Nil;
	}

    static inline Value Fox_SetField(VM* pVM, Value oInstance, const char* fieldName, Value value)
	{
        Value name = Fox_NewString(pVM, fieldName);
        Fox_AsInstance(oInstance)->fields.Set(Fox_AsString(name), value);

		return Fox_Nil;
	}

    static inline Value Fox_GetField(VM* pVM, Value oInstance, const char* fieldName)
	{
        Value value;
        Value name = Fox_NewString(pVM, fieldName);
        if (!Fox_AsInstance(oInstance)->fields.Get(Fox_AsString(name), value))
        {
            return Fox_Nil;
        }
		return value;
	}

	static inline void* Fox_GetUserData(Value oInstance)
	{
        return Fox_AsInstance(oInstance)->cStruct;
	}

	static inline void* Fox_SetUserData(Value oInstance, void* data)
	{
		Fox_AsInstance(oInstance)->cStruct = data;
        return Fox_AsInstance(oInstance)->cStruct;
	}

    static inline Value Fox_NewArray(VM* pVM)
	{
		return Fox_Object(pVM->gc.New<ObjectArray>());
	}
}

#endif