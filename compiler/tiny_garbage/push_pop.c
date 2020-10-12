/*
** EPITECH PROJECT, 2019
** push
** File description:
** push
*/

#include "garbage.h"

void gc_push(gc_t *vm, object_t *value)
{
    if (assert(vm->stack_size < STACK_MAX, "Stack overflow!"))
        return;
    for (int i = 0; i < vm->stack_size; i++)
        if (vm->stack[i] == NULL) {
            value->id = i;
            vm->stack[i] = value;
            return;
        }
    value->id = vm->stack_size;
    vm->stack[vm->stack_size++] = value;
}

object_t *pop(gc_t *vm)
{
    if (assert(vm->stack_size > 0, "Stack underflow!"))
        return NULL;
    vm->stack[vm->stack_size] = NULL;
    return (vm->stack[--vm->stack_size]);
}

void pushInt(gc_t *vm, int intValue)
{
    object_t *object = new_object(vm, OBJ_INT, sizeof(int));
    object->value = intValue;

    gc_push(vm, object);
}