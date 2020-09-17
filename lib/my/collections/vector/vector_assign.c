/*
** EPITECH PROJECT, 2019
** vector_assign
** File description:
** assign
*/

#include "private/vector_internal.h"
#include "my.h"

int vector_assign(vector *vector, size_t index, void *element)
{
    if (vector == NULL || element == NULL)
        return -1;
    if (vector->element_size == 0)
        return -1;
    if (index >= vector->size)
        return -1;

    vector__assign(vector, index, element);
    return 0;
}

void vector__assign(vector *vector, size_t index, void *element)
{
    void *offset = vector__offset(vector, index);
    my_memcpy(offset, element, vector->element_size);
}