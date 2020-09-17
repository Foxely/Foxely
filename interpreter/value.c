/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#include <stdlib.h>
#include "value.h"
#include "custom_memory.h"
#include "my.h"

void init_value_array(value_array *array)
{
    array->count = 0;
    array->capacity = 0;
    array->values = NULL;
}

void write_value_array(vm_t *vm, value_array *array, value val)
{
    if (array->capacity < array->count + 1) {
        int old_capacity = array->capacity;
        array->capacity = GROW_CAPACITY(old_capacity);
        array->values = GROW_ARRAY(vm, array->values, value,
                old_capacity, array->capacity);
    }

    array->values[array->count] = val;
    array->count++;
}

void free_value_array(vm_t *vm, value_array *array)
{
    FREE_ARRAY(vm, value, array->values, array->capacity);
    init_value_array(array);
}

void print_value(value val)
{
    switch (val.type) {
        case VAL_BOOL: printf(AS_BOOL(val) ? "true" : "false"); break;
        case VAL_NIL: printf("nil"); break;
        case VAL_NUMBER: printf("%g", AS_NUMBER(val)); break;
        case VAL_OBJ:    print_object(val); break;
    }
}

bool values_equal(value a, value b)
{
    if (a.type != b.type)
        return false;
    switch (a.type) {
        case VAL_BOOL:
            return AS_BOOL(a) == AS_BOOL(b);
        case VAL_NIL:
            return true;
        case VAL_NUMBER:
            return AS_NUMBER(a) == AS_NUMBER(b);
        case VAL_OBJ:
            return AS_OBJ(a) == AS_OBJ(b);
    }
    return false;
}