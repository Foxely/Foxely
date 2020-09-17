/*
** EPITECH PROJECT, 2019
** vector_foreach
** File description:
** loop a list
*/

#include <stdlib.h>
#include "my.h"
#include "private/vector_internal.h"

void vector_foreach(vector *v, int (*callback)(void *, void *), void *data)
{
    int result = 1;
    for (size_t i = 0; i < v->size && result; i++) {
        if (callback != NULL)
            result = callback(data, v->data[i]);
    }
}