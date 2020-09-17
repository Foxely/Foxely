/*
** EPITECH PROJECT, 2019
** vector_reallocate
** File description:
** reallocate the vector
*/

#include <stdlib.h>
#include "my.h"
#include "private/vector_internal.h"

int vector__reallocate(vector *vector, size_t new_capacity)
{
    size_t new_capacity_in_bytes;
    void *old;

    if (new_capacity < VECTOR_MINIMUM_CAPACITY) {
        if (vector->capacity > VECTOR_MINIMUM_CAPACITY)
            new_capacity = VECTOR_MINIMUM_CAPACITY;
        else
            return 0;
    }
    new_capacity_in_bytes = new_capacity * vector->element_size;
    old = vector->data;
    vector->data = malloc(new_capacity_in_bytes);
    if (vector->data == NULL)
        return -1;
    my_memcpy(vector->data, old, vector_byte_size(vector));
    vector->capacity = new_capacity;
    free(old);
    return 0;
}