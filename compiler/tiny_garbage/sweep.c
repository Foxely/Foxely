/*
** EPITECH PROJECT, 2019
** sweep
** File description:
** free address who I don't use
*/

#include "garbage.h"

void sweep(gc_t *vm)
{
    object_t **object = &vm->first_object;
    while (*object) {
        if ((*object)->marked == 0) {
            object_t *unreached = *object;
            *object = unreached->next;
            free(unreached);
            vm->num_objects--;
        } else {
            (*object)->marked = 0;
            object = &(*object)->next;
        }
    }
}