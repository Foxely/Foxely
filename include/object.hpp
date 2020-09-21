/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#pragma once

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "vm.hpp"
#include "chunk.hpp"
#include "value.hpp"

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

#define AS_CUSTOM(val)         ((obj_custom_field *)AS_OBJ(val))
#define AS_LIB(val)           ((obj_lib *)AS_OBJ(val))
#define AS_BOUND_METHOD(val)  ((obj_bound_method *)AS_OBJ(val))
#define AS_CLASS(val)         ((obj_class *)AS_OBJ(val))
#define AS_CLOSURE(val)       ((obj_closure *)AS_OBJ(val))
#define AS_FUNCTION(val)      ((ObjectFunction *)AS_OBJ(val))
#define AS_INSTANCE(val)        ((obj_instance *)AS_OBJ(val))
#define AS_NATIVE(val)        (((obj_native *)AS_OBJ(val))->function)
#define AS_STRING(val)        ((ObjectString *)AS_OBJ(val))
#define AS_CSTRING(val)       (((ObjectString *)AS_OBJ(val))->chars)

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

struct Object {
    ObjType type;
    bool is_marked;
    struct Object *next;
};

struct ObjectString {
    Object obj;
    int length;
    char *chars;
    uint32_t hash;
};
struct ObjectFunction {
    Object obj;
    int arity;
    int upValue_count;
    Chunk chunk;
    ObjectString *name;
};

struct ObjectUpValue {
    Object obj;
    Value *location;
    Value closed;
    struct ObjectUpValue *next;
};

typedef Value (*native_fn)(VM *vm, int arg_count, Value *args);

struct ObjectNative {
    Object obj;
    native_fn function;
};

struct ObjectClosure {
    Object obj;
    ObjectFunction *function;
    ObjectUpValue **upValues;
    int upValue_count;
};

struct ObjectClass {
    Object obj;
    ObjectString *name;
    // table_t methods;
};

struct ObjectInstance {
    Object obj;
    ObjectClass *klass;
    // table_t fields;
};


struct ObjectBoundMethod {
    Object obj;
    Value receiver;
    ObjectClosure *method;
};


typedef struct {
    char *name;
    native_fn function;
} lib_reg;

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



ObjectCustomField *
new_custom_field(VM *vm, char *type, void *field, field_destroyer destroy);

obj_lib *new_lib(VM *vm);
void define_lib(VM *vm, const char *name, lib_reg *regs);

ObjectBoundMethod *
new_bound_method(VM *vm, Value receiver, ObjectClosure *method);

ObjectInstance *new_instance(VM *vm, ObjectClass *klass);
ObjectClass *new_class(VM *vm, ObjectString *name);

ObjectUpValue *new_upValue(VM *vm, Value *slot);

ObjectClosure *new_closure(VM *vm, ObjectFunction *function);

ObjectNative *new_native(VM *vm, native_fn function);
// void define_native(VM *vm, table_t *table, const char *name, native_fn func);

ObjectFunction *new_function(VM *vm);
void print_function(ObjectFunction *function);

void concatenate(VM *vm);
void concatenate_number(VM *vm);
ObjectString *take_string(VM *vm, char *chars, int length);
ObjectString *copy_string(VM *vm, const char *chars, int length);
void print_object(Value val);
Object *allocate_object(VM *vm, size_t size, ObjType type);

static inline bool is_obj_type(Value val, ObjType type)
{
    return IS_OBJ(val) && AS_OBJ(val)->type == type;
}

#define ALLOCATE_OBJ(vm, type, object_type) \
    (type *)allocate_object(vm, sizeof(type), object_type)

#endif