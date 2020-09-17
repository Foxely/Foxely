/*
** EPITECH PROJECT, 2019
** list_foreach
** File description:
** loop a list
*/

#include <stdlib.h>
#include "list.h"
#include "private/list_internal.h"

list_iter_t list_iter(const list_t *self)
{
    list_iter_t iter = {0};

    iter.i = 0;
    iter.list = self;
    iter.next = self->head ? self->head->next : NULL;
    iter.v = self->head ? self->head->data : NULL;
    return (iter);
}

bool list_iter_end(const list_iter_t *iter)
{
    return (iter->i >= iter->list->len);
}

void list_iter_next(list_iter_t *iter)
{
    iter->i++;
    if (iter->i < iter->list->len) {
        iter->v = iter->next->data;
        iter->next = iter->next->next;
    }
}