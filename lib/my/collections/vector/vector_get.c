/*
** EPITECH PROJECT, 2019
** vector_get
** File description:
** get the data with the index we given
*/

#include "my.h"
#include "private/vector_internal.h"

void *vector_get(vector *vector, size_t index)
{
    if (vector == NULL)
        return NULL;
    if (index >= vector->size)
        return NULL;

    return vector->data[index];
}

void *vector_front(vector *vector)
{
    return vector_get(vector, 0);
}

void *vector_back(vector *vector)
{
    return vector_get(vector, vector->size - 1);
}