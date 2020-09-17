/*
** EPITECH PROJECT, 2019
** vector_reserve
** File description:
** reserve a memorry
*/

#include "my.h"
#include "private/vector_internal.h"

int vector_reserve(vector *vector, size_t minimum_capacity)
{
    if (minimum_capacity > vector->capacity)
        if (vector_resize(vector, minimum_capacity) == -1)
            return -1;
    return 0;
}
