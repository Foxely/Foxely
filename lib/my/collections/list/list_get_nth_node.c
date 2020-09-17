/*
** EPITECH PROJECT, 2019
** list_getnode_at
** File description:
** get the node on the index we given in params
*/

#include <stdlib.h>
#include "list.h"
#include "private/list_internal.h"

list_node_t *list__get_nth_node(list_node_t *v, int index)
{
    list_node_t *node = v;

    while (v && index--)
        node = node->next;
    return (node);
}