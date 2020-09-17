/*
** EPITECH PROJECT, 2019
** vector_offset
** File description:
** offset
*/

#include "my.h"
#include "private/vector_internal.h"

void *vector__offset(vector *vector, size_t index)
{
    return vector->data + (index * vector->element_size);
}