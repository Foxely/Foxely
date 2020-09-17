/*
** EPITECH PROJECT, 2019
** list_clear
** File description:
** clear a list
*/

#include "private/list_internal.h"
#include "list.h"

void list_clear(list_t *self)
{
    list_node_t *node = self->head;
    list_node_t *next = NULL;

    if (!node)
        return;
    node->prev->next = NULL;
    while (node != NULL) {
        next = node->next;
        free(node);
        node = next;
    }
    self->len = 0;
    self->head = NULL;
}

void list_clear_with(list_t *self, list_iter_fn_t *destroyer, void *data)
{
    if (destroyer != NULL)
        list_foreach(self, destroyer, data);
    list_clear(self);
}