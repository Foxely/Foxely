/*
** EPITECH PROJECT, 2019
** vector_init
** File description:
** initialize a new vector
*/

#include <stdlib.h>
#include "my.h"
#include "debug.h"
#include "private/vector_internal.h"

int vector_init(vector *vector, size_t capacity)
{
    if (vector == NULL) {
        my_putstr("vector not be null\n");
        return -1;
    }

    vector->size = 0;
    vector->capacity = MAX(VECTOR_MINIMUM_CAPACITY, capacity);
    vector->data = malloc(vector->capacity * sizeof(void *));
    return vector->data == NULL ? -1 : 0;
}