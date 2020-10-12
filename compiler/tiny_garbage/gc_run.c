/*
** EPITECH PROJECT, 2019
** gc_run
** File description:
** run the GC
*/

#include "garbage.h"
#include "stdio.h"

void gc_run(gc_t *vm)
{
    int num_objects = vm->num_objects;

    mark_all(vm);
    sweep(vm);
    vm->max_objects = vm->num_objects * 2;
    int collected = num_objects - vm->num_objects;
    printf("Collected %d objects, %d remaining.\n", collected, vm->num_objects);
}

void gc_print(gc_t *vm)
{
    for (int i = 0; i < vm->stack_size; i++) {
        if (vm->stack[i])
            printf("%p\n", vm->stack[i]->data);
    }
}