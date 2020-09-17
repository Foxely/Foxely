/*
** EPITECH PROJECT, 2019
** vector_resize
** File description:
** resize the vector
*/

#include "my.h"
#include "private/vector_internal.h"
#include "memory.h"

int vector_resize(vector *vector, size_t new_size)
{
    int  status = -1;
    if (vector) {
        void **items = my_realloc(vector->data,
                                sizeof(void *) * vector->size,
                                sizeof(void *) * new_size);
        if (items) {
            vector->data = items;
            vector->capacity = new_size;
            status = 0;
        }
    }
    return status;
}