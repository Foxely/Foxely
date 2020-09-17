/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#ifndef __VALUE_H__
#define __VALUE_H__

#include "type.h"
#include "common.h"

typedef struct s_obj obj_t;
typedef struct s_obj_string obj_string;
typedef struct s_obj_function obj_function;
typedef struct s_obj_closure obj_closure;
typedef struct s_obj_upvalue obj_upvalue;
typedef struct s_obj_class obj_class;

typedef enum {
    VAL_BOOL,
    VAL_NIL,
    VAL_NUMBER,
    VAL_OBJ
} value_type;

typedef struct s_value {
    value_type type;
    union {
        bool boolean;
        double number;
        obj_t *obj;
    } as;
} value;

typedef struct {
    int capacity;
    int count;
    value *values;
} value_array;

#define IS_BOOL(val)    ((val).type == VAL_BOOL)
#define IS_NIL(val)     ((val).type == VAL_NIL)
#define IS_NUMBER(val)  ((val).type == VAL_NUMBER)
#define IS_OBJ(val)     ((val).type == VAL_OBJ)

#define AS_OBJ(val)     ((val).as.obj)
#define AS_BOOL(val)    ((val).as.boolean)
#define AS_NUMBER(val)  ((val).as.number)

#define BOOL_VAL(val)   ((value){ VAL_BOOL, { .boolean = val } })
#define NIL_VAL           ((value){ VAL_NIL, { .number = 0 } })
#define NUMBER_VAL(val) ((value){ VAL_NUMBER, { .number = val } })
#define OBJ_VAL(object)   ((value){ VAL_OBJ, { .obj = (obj_t *)object } })

bool values_equal(value a, value b);
void init_value_array(value_array* array);
void write_value_array(vm_t *vm, value_array *array, value value);
void free_value_array(vm_t *vm, value_array *array);
void print_value(value val);

#endif