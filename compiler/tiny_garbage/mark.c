/*
** EPITECH PROJECT, 2019
** mark
** File description:
** mark memory
*/

#include "garbage.h"

void mark(object_t *object)
{
    if (object->marked) return;

    object->marked = 1;
}

void mark_all(gc_t *vm)
{
    for (int i = 0; i < vm->stack_size; i++)
        if (vm->stack[i] != NULL)
            mark(vm->stack[i]);
}