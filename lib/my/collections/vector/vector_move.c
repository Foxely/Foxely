/*
** EPITECH PROJECT, 2019
** vector_move
** File description:
** move
*/

#include "my.h"
#include "private/vector_internal.h"
#include "memory.h"

int vector__move_right(vector *vector, size_t index)
{
    void *offset = vector__offset(vector, index);
    size_t elements_in_bytes = (vector->size - index) * vector->element_size;
    my_memmove(offset + vector->element_size, offset, elements_in_bytes);
    return 0;
}

void vector__move_left(vector *vector, size_t index)
{
    size_t right_elem_in_bytes;
    void *offset;

    offset = vector__offset(vector, index);
    right_elem_in_bytes = (vector->size - index - 1) * vector->element_size;
    my_memmove(offset, offset + vector->element_size, right_elem_in_bytes);
}

int vector_move(vector *destination, vector *source)
{
    if (destination == NULL)
        return -1;
    if (source == NULL)
        return -1;

    *destination = *source;
    source->data = NULL;

    return 0;
}

int vector_move_assign(vector *destination, vector *source)
{
    vector_swap(destination, source);
    return vector_destroy(source);
}