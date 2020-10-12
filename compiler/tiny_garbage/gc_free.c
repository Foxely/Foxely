/*
** EPITECH PROJECT, 2019
** my_free
** File description:
** like free funct
*/

#include "garbage.h"

static object_t *get_object(void *ptr)
{
    return ptr - sizeof(object_t);
}

void gc_free(gc_t *vm, void *ptr)
{
    if (assert(vm->stack_size > 0, "Stack underflow!")) return;
    if (assert(ptr != NULL, "Cannot free a Null pointer!")) return;

    object_t *object = get_object(ptr);
    vm->stack[object->id] = NULL;
}