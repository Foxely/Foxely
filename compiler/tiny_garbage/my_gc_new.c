/*
** EPITECH PROJECT, 2019
** mark_sweep
** File description:
** sf
*/

#include <stdio.h>
#include <stdlib.h>
#include "garbage.h"

static gc_t *gc_static;

int assert(int condition, const char *message) {
    if (!condition) {
        printf("%s\n", message);
        return (1);
    }
    return (0);
}

static size_t hash(void *ptr)
{
    return ((unsigned long)ptr) >> 3;
}

gc_t *get_garbage(void)
{
    return (my_gc_new());
}

gc_t *my_gc_new(void)
{
    if (gc_static != NULL)
        return (gc_static);
    gc_t *vm = malloc(sizeof(gc_t));
    vm->stack_size = 0;
    vm->first_object = NULL;
    vm->num_objects = 0;
    vm->max_objects = 8;
    gc_static = vm;
    return (vm);
}

object_t *new_object(gc_t *vm, object_type type, size_t size)
{
    if (vm->num_objects >= vm->max_objects)
        gc_run(vm);
    object_t *object = malloc(sizeof(object_t) + size);
    object->type = type;
    object->next = vm->first_object;
    vm->first_object = object;
    object->marked = 0;
    vm->num_objects++;
    return (object);
}