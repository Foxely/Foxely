/*
** EPITECH PROJECT, 2019
** vector_adjust_capacity
** File description:
** adjust capacity
*/

#include "private/vector_internal.h"
#include "vector.h"

int vector__adjust_capacity(vector *vector)
{
    return vector__reallocate(vector, MAX(1, vector->size * VECTOR_GROW_FACT));
}