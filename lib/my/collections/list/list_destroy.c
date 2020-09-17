/*
** EPITECH PROJECT, 2019
** list_delete
** File description:
** Delete a node
*/

#include <stdlib.h>
#include "list.h"
#include "private/list_internal.h"

void list_destroy(list_t *self)
{
    list_node_t *node = self->head;
    list_node_t *next = NULL;

    if (node != NULL) {
        node->prev->next = NULL;
        while (node != NULL) {
            next = node->next;
            free(node);
            node = next;
        }
    }
    free(self);
}

void list_destroy_with(list_t *self, list_iter_fn_t *destroyer, void *user_data)
{
    if (destroyer != NULL)
        list_foreach(self, destroyer, user_data);
    list_destroy(self);
}