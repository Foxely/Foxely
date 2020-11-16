/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#pragma once

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <iostream>
#include <string>
#include <functional>

#include "chunk.hpp"
#include "value.hpp"
#include "gc.hpp"
#include "Table.hpp"
#include "Map.hpp"

class VM;

#define Fox_ObjectType(val)         (Fox_AsObject(val)->type)


#define Fox_IsMap(val)        is_obj_type(val, OBJ_MAP)
#define Fox_IsArray(val)        is_obj_type(val, OBJ_ARRAY)
#define Fox_IsAbstract(val)        is_obj_type(val, OBJ_ABSTRACT)
#define Fox_IsLib(val)           is_obj_type(val, OBJ_LIB)
#define Fox_IsBoundMethod(val)  is_obj_type(val, OBJ_BOUND_METHOD)
#define Fox_IsClass(val)         is_obj_type(val, OBJ_CLASS)
#define Fox_IsClosure(val)       is_obj_type(val, OBJ_CLOSURE)
#define Fox_IsInstance(val)      is_obj_type(val, OBJ_INSTANCE)
#define Fox_IsFunction(val)      is_obj_type(val, OBJ_FUNCTION)
#define Fox_IsNative(val)        is_obj_type(val, OBJ_NATIVE)
#define Fox_IsString(val)        is_obj_type(val, OBJ_STRING)
#define Fox_IsModule(val)        is_obj_type(val, OBJ_MODULE)

#define Fox_AsMap(val)           ((ObjectMap *)Fox_AsObject(val))
#define Fox_AsArray(val)           ((ObjectArray *)Fox_AsObject(val))
#define Fox_AsAbstract(val)        ((ObjectAbstract *)Fox_AsObject(val))
#define Fox_AsLib(val)           	((ObjectLib *)Fox_AsObject(val))
#define Fox_AsBoundMethod(val)  	((ObjectBoundMethod *)Fox_AsObject(val))
#define Fox_AsClass(val)         	((ObjectClass *)Fox_AsObject(val))
#define Fox_AsClosure(val)       	((ObjectClosure *)Fox_AsObject(val))
#define Fox_AsFunction(val)      	((ObjectFunction *)Fox_AsObject(val))
#define Fox_AsInstance(val)        ((ObjectInstance *)Fox_AsObject(val))
#define Fox_AsNative(val)        	(((ObjectNative *)Fox_AsObject(val))->function)
#define Fox_AsString(val)        	((ObjectString *)Fox_AsObject(val))
#define Fox_AsCString(val)       	(((ObjectString *)Fox_AsObject(val))->string.c_str())
#define Fox_AsModule(val)       	((ObjectModule *)Fox_AsObject(val))

typedef enum {
    OBJ_UNKNOWN,
    OBJ_ARRAY,
    OBJ_MAP,
    OBJ_ABSTRACT,
    OBJ_BOUND_METHOD,
    OBJ_CLASS,
    OBJ_CLOSURE,
    OBJ_FUNCTION,
    OBJ_INSTANCE,
    OBJ_NATIVE,
    OBJ_LIB,
    OBJ_STRING,
    OBJ_UPVALUE,
    OBJ_MODULE,
    OBJ_HANDLE,
} ObjType;



class Object : public Traceable
{
public:
    ObjType type;
    bool bIsForeign;

    bool operator==(const Object& other) const
    {
        return type == other.type;
    }
};

class ObjectString : public Object
{
public:
    explicit ObjectString(const std::string& v) : string(v) {}

    uint32_t hash;
    std::string string;
};

// A loaded module and the top-level variables it defines.
//
// While this is an Object and is managed by the GC, it never appears as a
// first-class object in Foxely.
class ObjectModule : public Object
{
public:
    explicit ObjectModule(ObjectString* strName)
	{
		type = OBJ_MODULE;
        m_strName = strName;
	}

    bool operator==(const ObjectModule& other) const
    {
        return m_strName == other.m_strName;
    }

    Table m_vVariables;
    // The name of the module.
    ObjectString* m_strName;
};

class ObjectFunction : public Object
{
public:
    int arity;
    int upValueCount;
    Chunk chunk;
    ObjectString *name;
    ObjectModule* module;

	explicit ObjectFunction()
	{
		arity = 0;
		name = NULL;
		upValueCount = 0;
		type = OBJ_FUNCTION;
        module = NULL;
	}
};

class ObjectUpvalue : public Object
{
public:
    Value *location;
    Value closed;
    struct ObjectUpvalue *next;

    explicit ObjectUpvalue(Value* slot)
    {
        type = OBJ_UPVALUE;
        location = slot;
        next = NULL;
        closed = Fox_Nil;
    }
};

using NativeFn = std::function<Value(VM*, int, Value*)>;

class ObjectNative : public Object
{
public:
    NativeFn function;

	explicit ObjectNative(NativeFn func)
	{
		function = func;
		type = OBJ_NATIVE;
	}

    explicit ObjectNative(NativeFn func, int a)
	{
		function = func;
		type = OBJ_NATIVE;
	}
};

class ObjectLib : public Object
{
public:
    ObjectString *name;
    Table methods;

	explicit ObjectLib(ObjectString* n)
	{
		type = OBJ_LIB;
		name = NULL;
		name = n;
		methods = Table();
	}
};

class ObjectClosure : public Object
{
public:
    ObjectFunction *function;
    ObjectUpvalue **upValues;
    int upvalueCount;

    ObjectClosure(VM* oVM, ObjectFunction *func);
};

class ObjectClass : public Object
{
public:
    ObjectClass* superClass;
    ObjectString *name;
    Table methods;
    int derivedCount;

	explicit ObjectClass(ObjectString* n)
	{
		type = OBJ_CLASS;
		name = n;
		methods = Table();
        superClass = NULL;
        derivedCount = 0;
	}

    bool operator==(const ObjectClass& other) const
    {
        ObjectClass* cl = (ObjectClass*) this;
        ObjectClass* end = (ObjectClass*) &other;
        if (derivedCount < other.derivedCount)
        {
            cl = (ObjectClass*) &other;
            end = (ObjectClass*) this;
        }
        while (cl && !(cl == end))
            cl = cl->superClass;
        
        return cl != NULL;
    }
};

class ObjectInstance : public Object
{
public:
    ObjectClass *klass;
    Table fields;
    Table getters;
    Table setters;
	void* cStruct;

	explicit ObjectInstance(ObjectClass *k)
	{
		type = OBJ_INSTANCE;
		klass = k;
		fields = Table();
        cStruct = nullptr;
	}

    explicit ObjectInstance(ObjectClass *k, void* cS)
	{
		type = OBJ_INSTANCE;
		klass = k;
		fields = Table();
        cStruct = cS;
	}

    bool operator==(const ObjectInstance& other) const
    {
        return *klass == *other.klass && fields.m_vEntries == other.fields.m_vEntries;
    }
};

class ObjectBoundMethod : public Object
{
public:
    Value receiver;
    ObjectClosure *method;

	explicit ObjectBoundMethod(Value r, ObjectClosure* m)
	{
		type = OBJ_BOUND_METHOD;
		receiver = r;
		method = m;
	}
};

struct ObjectAbstractType
{
    const char *name;
};

class ObjectAbstract : public Object
{
public:
    ObjectAbstractType* abstractType;
    void* data;

    explicit ObjectAbstract()
	{
		type = OBJ_ABSTRACT;
        abstractType = NULL;
        data = NULL;
	}

    explicit ObjectAbstract(void* d, ObjectAbstractType* aType)
	{
		type = OBJ_ABSTRACT;
        abstractType = aType;
        data = d;
	}

    bool operator==(const ObjectAbstract& other) const
    {
        return data == other.data && abstractType == other.abstractType;
    }
};

class ObjectArray : public Object
{
public:
    std::vector<Value> m_vValues;

    explicit ObjectArray()
	{
		type = OBJ_ARRAY;
        m_vValues = std::vector<Value>();
	}

    bool operator==(const ObjectArray& other) const
    {
        return m_vValues == other.m_vValues;
    }
};

class ObjectMap : public Object
{
public:
    MapTable m_vValues;
    Table m_oMethods;

    explicit ObjectMap()
	{
		type = OBJ_MAP;
        m_vValues = MapTable();
	}

    bool operator==(const ObjectMap& other) const
    {
        return m_vValues.m_vEntries == other.m_vValues.m_vEntries;
    }
};

static inline bool is_obj_type(Value val, ObjType type)
{
    return Fox_IsObject(val) && Fox_AsObject(val)->type == type;
}

#endif