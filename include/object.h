/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "type.h"
#include "value.h"
#include "vm.h"
#include "chunk.h"
#include "table.h"

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
#define AS_FUNCTION(val)      ((obj_function *)AS_OBJ(val))
#define AS_INSTANCE(val)        ((obj_instance *)AS_OBJ(val))
#define AS_NATIVE(val)        (((obj_native *)AS_OBJ(val))->function)
#define AS_STRING(val)        ((obj_string *)AS_OBJ(val))
#define AS_CSTRING(val)       (((obj_string *)AS_OBJ(val))->chars)

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
} obj_type;

struct s_obj {
    obj_type type;
    bool is_marked;
    struct s_obj *next;
};

struct s_obj_string {
    obj_t obj;
    int length;
    char *chars;
    uint32_t hash;
};
struct s_obj_function {
    obj_t obj;
    int arity;
    int upvalue_count;
    chunk_t chunk;
    obj_string *name;
};

struct s_obj_upvalue {
    obj_t obj;
    value *location;
    value closed;
    struct s_obj_upvalue *next;
};

typedef value (*native_fn)(vm_t *vm, int arg_count, value *args);

typedef struct {
    obj_t obj;
    native_fn function;
} obj_native;

struct s_obj_closure {
    obj_t obj;
    obj_function *function;
    obj_upvalue **upvalues;
    int upvalue_count;
};

struct s_obj_class {
    obj_t obj;
    obj_string *name;
    table_t methods;
};

typedef struct {
    obj_t obj;
    obj_class *klass;
    table_t fields;
} obj_instance;


typedef struct {
    obj_t obj;
    value receiver;
    obj_closure *method;
} obj_bound_method;


typedef struct {
    char *name;
    native_fn function;
} lib_reg;

typedef struct {
    obj_t obj;
    table_t fields;
} obj_lib;


typedef void (*field_destroyer)(void *);
typedef struct {
    obj_t obj;
    obj_string *type;
    void *field;
    field_destroyer destroy;
} obj_custom_field;



obj_custom_field *
new_custom_field(vm_t *vm, char *type, void *field, field_destroyer destroy);

obj_lib *new_lib(vm_t *vm);
void define_lib(vm_t *vm, const char *name, lib_reg *regs);

obj_bound_method *
new_bound_method(vm_t *vm, value receiver, obj_closure *method);

obj_instance *new_instance(vm_t *vm, obj_class *klass);
obj_class *new_class(vm_t *vm, obj_string *name);

obj_upvalue *new_upvalue(vm_t *vm, value *slot);

obj_closure *new_closure(vm_t *vm, obj_function *function);

obj_native *new_native(vm_t *vm, native_fn function);
void define_native(vm_t *vm, table_t *table, const char *name, native_fn func);

obj_function *new_function(vm_t *vm);
void print_function(obj_function *function);

void concatenate(vm_t *vm);
void concatenate_number(vm_t *vm);
obj_string *take_string(vm_t *vm, char *chars, int length);
obj_string *copy_string(vm_t *vm, const char *chars, int length);
void print_object(value val);
obj_t *allocate_object(vm_t *vm, size_t size, obj_type type);

static inline bool is_obj_type(value val, obj_type type)
{
    return IS_OBJ(val) && AS_OBJ(val)->type == type;
}

#define ALLOCATE_OBJ(vm, type, object_type) \
    (type *)allocate_object(vm, sizeof(type), object_type)

#endif