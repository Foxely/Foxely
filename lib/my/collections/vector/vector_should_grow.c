/*
** EPITECH PROJECT, 2019
** vector_should_grow
** File description:
** should grow the vector
*/

#include "my.h"
#include "private/vector_internal.h"

int vector__should_grow(vector *vector)
{
    return vector->size == vector->capacity;
}