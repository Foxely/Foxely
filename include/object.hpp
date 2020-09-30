/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#pragma once

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <string>

#include "chunk.hpp"
#include "value.hpp"
#include "gc.hpp"
#include "Table.hpp"

#define OBJ_TYPE(val)         (AS_OBJ(val)->type)


#define IS_CUSTOM(val)        is_obj_type(val, OBJ_CUSTOM)
#define IS_LIB(val)           is_obj_type(val, OBJ_LIB)
#define IS_BOUND_METHOD(val)  is_obj_type(val, OBJ_BOUND_METHOD)
#define IS_CLASS(val)         is_obj_type(val, OBJ_CLASS)
#define IS_CLOSURE(val)       is_obj_type(val, OBJ_CLOSURE)
#define IS_INSTANCE(val)      is_obj_type(val, OBJ_INSTANCE)
#define IS_FUNCTION(val)      is_obj_type(val, OBJ_FUNCTION)
#define IS_NATIVE(val)        is_obj_type(val, OBJ_NATIVE)
#define IS_STRING(val)        is_obj_type(val, OBJ_STRING)

#define AS_CUSTOM(val)         ((ObjectCustomField *)AS_OBJ(val))
#define AS_LIB(val)           ((ObjectLib *)AS_OBJ(val))
#define AS_BOUND_METHOD(val)  ((ObjectBoundMethod *)AS_OBJ(val))
#define AS_CLASS(val)         ((ObjectClass *)AS_OBJ(val))
#define AS_CLOSURE(val)       ((ObjectClosure *)AS_OBJ(val))
#define AS_FUNCTION(val)      ((ObjectFunction *)AS_OBJ(val))
#define AS_INSTANCE(val)        ((ObjectInstance *)AS_OBJ(val))
#define AS_NATIVE(val)        (((ObjectNative *)AS_OBJ(val))->function)
#define AS_STRING(val)        ((ObjectString *)AS_OBJ(val))
#define AS_CSTRING(val)       (((ObjectString *)AS_OBJ(val))->string.c_str())

typedef enum {
    OBJ_BOUND_METHOD,
    OBJ_CLASS,
    OBJ_CLOSURE,
    OBJ_FUNCTION,
    OBJ_INSTANCE,
    OBJ_NATIVE,
    OBJ_LIB,
    OBJ_STRING,
    OBJ_UPVALUE,
    OBJ_CUSTOM,
} ObjType;



class Object : public Traceable
{
public:
    ObjType type;
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
    ObjectString *name;
    Table methods;

	explicit ObjectClass(ObjectString* n)
	{
		type = OBJ_CLASS;
		name = n;
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
};


class ObjectBoundMethod : public Object
{
    Value receiver;
    ObjectClosure *method;
};


// typedef struct {
//     char *name;
//     native_fn function;
// } lib_reg;

typedef struct {
    Object obj;
    // table_t fields;
} obj_lib;


typedef void (*field_destroyer)(void *);
struct ObjectCustomField {
    Object obj;
    ObjectString *type;
    void *field;
    field_destroyer destroy;
};



// ObjectCustomField *
// new_custom_field(VM *vm, char *type, void *field, field_destroyer destroy);

// obj_lib *new_lib(VM *vm);
// void define_lib(VM *vm, const char *name, lib_reg *regs);

// ObjectBoundMethod *
// new_bound_method(VM *vm, Value receiver, ObjectClosure *method);

// ObjectInstance *new_instance(VM *vm, ObjectClass *klass);
// ObjectClass *new_class(VM *vm, ObjectString *name);

// ObjectUpValue *new_upValue(VM *vm, Value *slot);

// ObjectClosure *new_closure(VM *vm, ObjectFunction *function);

// ObjectNative *new_native(VM *vm, native_fn function);
// // void define_native(VM *vm, table_t *table, const char *name, native_fn func);

ObjectFunction *NewFunction();
// void print_function(ObjectFunction *function);

// void concatenate_number(VM *vm);
// ObjectString* takeString(const std::string& value);
// ObjectString* copyString(const std::string& value);
// Object* allocateObject(ObjType type);

static inline bool is_obj_type(Value val, ObjType type)
{
    return IS_OBJ(val) && AS_OBJ(val)->type == type;
}

#define ALLOCATE_OBJ(type, objectType) \
    static_cast<type*>(allocateObject(objectType))

#endif
