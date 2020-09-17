/*
** EPITECH PROJECT, 2019
** vector_copy
** File description:
** copy a vector
*/

#include <stdlib.h>
#include "my.h"
#include "private/vector_internal.h"

int vector_copy(vector *dest, vector *src)
{
    if (dest == NULL || src == NULL)
        return -1;
    if (vector_is_initialized(dest))
        return -1;
    if (!vector_is_initialized(src))
        return -1;

    dest->size = src->size;
    dest->capacity = src->size * 2;
    dest->element_size = src->element_size;
    dest->data = malloc(dest->capacity * src->element_size);
    if (dest->data == NULL)
        return -1;
    my_memcpy(dest->data, src->data, vector_byte_size(src));
    return 0;
}