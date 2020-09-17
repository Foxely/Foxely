/*
** EPITECH PROJECT, 2019
** vector_byte_size
** File description:
** return the byte size of the data
*/

#include <stdlib.h>
#include "my.h"
#include "private/vector_internal.h"

size_t vector_byte_size(const vector *vector)
{
    return vector->size * vector->element_size;
}