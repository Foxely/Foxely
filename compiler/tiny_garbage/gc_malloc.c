/*
** EPITECH PROJECT, 2019
** my_malloc
** File description:
** like malloc func
*/

#include "garbage.h"

void *gc_malloc(gc_t *vm, size_t size)
{
    object_t *object = new_object(vm, OBJ_VOID_PTR, size);
    object->data  = object + 1;
    gc_push(vm, object);
    return (object->data);
}