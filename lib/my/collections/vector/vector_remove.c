/*
** EPITECH PROJECT, 2019
** vector_remove
** File description:
** remove the vector
*/

#include "my.h"
#include "private/vector_internal.h"

int vector_remove(vector *vector, size_t index)
{
    if (vector == NULL || vector->element_size == 0)
        return -1;
    if (index >= vector->size)
        return -1;

    vector__move_left(vector, index);

#ifndef VECTOR_NO_SHRINK
    if (--vector->size == vector->capacity / 4)
        vector__adjust_capacity(vector);
#endif

    return 0;
}
