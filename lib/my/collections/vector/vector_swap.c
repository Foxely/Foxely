/*
** EPITECH PROJECT, 2019
** vector_swap
** File description:
** swap
*/

#include "my.h"
#include "private/vector_internal.h"

int vector_swap(vector *destination, vector *source)
{
    void *temp;

    if (destination == NULL)
        return -1;
    if (source == NULL)
        return -1;
    if (!vector_is_initialized(destination))
        return -1;
    if (!vector_is_initialized(source))
        return -1;

    vector__swap(&destination->size, &source->size);
    vector__swap(&destination->capacity, &source->capacity);
    vector__swap(&destination->element_size, &source->element_size);

    temp = destination->data;
    destination->data = source->data;
    source->data = temp;

    return 0;
}

void vector__swap(size_t *first, size_t *second)
{
    size_t temp = *first;
    *first = *second;
    *second = temp;
}