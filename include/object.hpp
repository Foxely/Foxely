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

#include "chunk.hpp"
#include "value.hpp"
#include "gc.hpp"
#include "Table.hpp"

#define OBJ_TYPE(val)         (AS_OBJ(val)->type)


#define IS_ARRAY(val)        is_obj_type(val, OBJ_ARRAY)
#define IS_ABSTRACT(val)        is_obj_type(val, OBJ_ABSTRACT)
#define IS_LIB(val)           is_obj_type(val, OBJ_LIB)
#define IS_BOUND_METHOD(val)  is_obj_type(val, OBJ_BOUND_METHOD)
#define IS_CLASS(val)         is_obj_type(val, OBJ_CLASS)
#define IS_NATIVE_CLASS(val)         is_obj_type(val, OBJ_NATIVE_CLASS)
#define IS_LIB(val)         is_obj_type(val, OBJ_LIB)
#define IS_CLOSURE(val)       is_obj_type(val, OBJ_CLOSURE)
#define IS_INSTANCE(val)      is_obj_type(val, OBJ_INSTANCE)
#define IS_NATIVE_INSTANCE(val)      is_obj_type(val, OBJ_NATIVE_INSTANCE)
#define IS_ABSTRACT_INSTANCE(val)      is_obj_type(val, OBJ_ABSTRACT_INSTANCE)
#define IS_FUNCTION(val)      is_obj_type(val, OBJ_FUNCTION)
#define IS_NATIVE(val)        is_obj_type(val, OBJ_NATIVE)
#define IS_STRING(val)        is_obj_type(val, OBJ_STRING)

#define AS_ARRAY(val)           ((ObjectArray *)AS_OBJ(val))
#define AS_ABSTRACT(val)        ((ObjectAbstract *)AS_OBJ(val))
#define AS_LIB(val)           	((ObjectLib *)AS_OBJ(val))
#define AS_BOUND_METHOD(val)  	((ObjectBoundMethod *)AS_OBJ(val))
#define AS_CLASS(val)         	((ObjectClass *)AS_OBJ(val))
#define AS_NATIVE_CLASS(val)	((ObjectNativeClass *)AS_OBJ(val))
#define AS_LIB(val)         	((ObjectLib *)AS_OBJ(val))
#define AS_CLOSURE(val)       	((ObjectClosure *)AS_OBJ(val))
#define AS_FUNCTION(val)      	((ObjectFunction *)AS_OBJ(val))
#define AS_INSTANCE(val)        ((ObjectInstance *)AS_OBJ(val))
#define AS_NATIVE_INSTANCE(val) ((ObjectNativeInstance *)AS_OBJ(val))
#define AS_ABSTRACT_INSTANCE(val) ((ObjectAbstractInstance *)AS_OBJ(val))
#define AS_NATIVE(val)        	(((ObjectNative *)AS_OBJ(val))->function)
#define AS_STRING(val)        	((ObjectString *)AS_OBJ(val))
#define AS_CSTRING(val)       	(((ObjectString *)AS_OBJ(val))->string.c_str())

typedef enum {
    OBJ_ARRAY,
    OBJ_ABSTRACT,
    OBJ_BOUND_METHOD,
    OBJ_CLASS,
    OBJ_CLOSURE,
    OBJ_FUNCTION,
    OBJ_INSTANCE,
    OBJ_NATIVE_INSTANCE,
	OBJ_ABSTRACT_INSTANCE,
    OBJ_NATIVE,
    OBJ_NATIVE_CLASS,
    OBJ_LIB,
    OBJ_STRING,
    OBJ_UPVALUE,
} ObjType;



class Object : public Traceable
{
public:
    ObjType type;

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

class ObjectFunction : public Object
{
public:
    int arity;
    int upValueCount;
    Chunk chunk;
    ObjectString *name;

	explicit ObjectFunction()
	{
		arity = 0;
		name = NULL;
		upValueCount = 0;
		type = OBJ_FUNCTION;
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
        closed = NIL_VAL;
    }
};

typedef Value (*NativeFn)(int arg_count, Value *args);

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

class ObjectNativeClass : public Object
{
public:
    ObjectString *name;
    Table methods;

	explicit ObjectNativeClass(ObjectString* n)
	{
		type = OBJ_NATIVE_CLASS;
		name = n;
		methods = Table();
	}
};

class ObjectNativeInstance : public Object
{
public:
    ObjectNativeClass *klass;
    Table fields;

	explicit ObjectNativeInstance(ObjectNativeClass *k)
	{
		type = OBJ_NATIVE_INSTANCE;
		klass = k;
		fields = Table();
	}

    bool operator==(const ObjectNativeInstance& other) const
    {
        return klass == other.klass && fields.m_vEntries == other.fields.m_vEntries;
    }
};

class ObjectAbstractInstance : public Object
{
public:
    ObjectString* name;
    void* data;
    Table methods;
    Table fields;

	explicit ObjectAbstractInstance(ObjectString* n)
	{
		type = OBJ_ABSTRACT_INSTANCE;
		name = n;
		fields = Table();
		methods = Table();
	}

    bool operator==(const ObjectAbstractInstance& other) const
    {
        return name->string == other.name->string && fields.m_vEntries == other.fields.m_vEntries;
    }
};

class ObjectClosure : public Object
{
public:
    ObjectFunction *function;
    ObjectUpvalue **upValues;
    int upvalueCount;

    explicit ObjectClosure(ObjectFunction *func)
    {
        type = OBJ_CLOSURE;
        function = func;
        upvalueCount = func->upValueCount;
        upValues = new ObjectUpvalue*[func->upValueCount];
        for (int i = 0; i < func->upValueCount; i++) {
            upValues[i] = NULL;
        }
    }
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

	explicit ObjectInstance(ObjectClass *k)
	{
		type = OBJ_INSTANCE;
		klass = k;
		fields = Table();
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

static inline bool is_obj_type(Value val, ObjType type)
{
    return IS_OBJ(val) && AS_OBJ(val)->type == type;
}

#define ALLOCATE_OBJ(type, objectType) \
    static_cast<type*>(allocateObject(objectType))

#endif
