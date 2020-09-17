/*
** EPITECH PROJECT, 2019
** list_foreach
** File description:
** loop a list
*/

#include <stdlib.h>
#include "list.h"
#include "private/list_internal.h"

int list_foreach(list_t *v, int (*callback)(void *, void *), void *data)
{
    int err = 0;

    for (list_iter_t iter = list_iter(v); \
        !list_iter_end(&iter) && (err == 0); list_iter_next(&iter))
        err = callback(data, iter.v);
    return (err);
}