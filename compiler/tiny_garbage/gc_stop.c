/*
** EPITECH PROJECT, 2019
** gc_stop
** File description:
** stop the GC
*/

#include "garbage.h"

void gc_stop(gc_t *vm)
{
    vm->stack_size = 0;
    gc_run(vm);
    free(vm);
}