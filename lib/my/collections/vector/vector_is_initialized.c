/*
** EPITECH PROJECT, 2019
** vector_is_initialized
** File description:
** return 1 if is initialize and 0 if not
*/

#include "my.h"
#include "private/vector_internal.h"

int vector_is_initialized(vector *vector)
{
    return vector->data != NULL;
}

size_t vector_free_space(const vector *vector)
{
    return vector->capacity - vector->size;
}

int vector_is_empty(const vector *vector)
{
    return vector->size == 0;
}