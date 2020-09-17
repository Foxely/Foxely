/*
** EPITECH PROJECT, 2019
** list_search
** File description:
** search in a list
*/

#include "list.h"
#include "private/list_internal.h"
#include "my.h"

void *list_find_with(list_t *self, list_iter_fn_t *fn, void *element)
{
    list_node_t *node = self->head;

    if (!node)
        return (NULL);
    for (usize_t i = 0; i < self->len; i++, node = node->next) {
        if (fn ? !fn(element, node->data) : (element == node->data))
            return (node->data);
    }
    return (NULL);
}

void *list_find(list_t *self, void *element)
{
    void *data = list_find_with(self, NULL, element);
    return (data);
}