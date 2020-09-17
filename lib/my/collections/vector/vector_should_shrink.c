/*
** EPITECH PROJECT, 2019
** vector_should_shrink
** File description:
** should
*/

#include "my.h"
#include "private/vector_internal.h"

int vector__should_shrink(vector *vector)
{
    return vector->size == vector->capacity * VECTOR_SHRINK_THRESHOLD;
}

int vector_shrink_to_fit(vector *vector)
{
    return vector__reallocate(vector, vector->size);
}