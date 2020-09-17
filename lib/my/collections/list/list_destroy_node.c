/*
** EPITECH PROJECT, 2019
** _list_destroy_node
** File description:
** destroy a node
*/

#include "private/list_internal.h"
#include "list.h"

void *list__destroy_node(list_node_t *node)
{
    void *data = node->data;

    node->prev->next = node->next;
    node->next->prev = node->prev;
    free(node);
    return (data);
}