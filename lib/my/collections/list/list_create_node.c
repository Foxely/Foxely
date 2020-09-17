/*
** EPITECH PROJECT, 2019
** list_insert
** File description:
** insert a node in a list
*/

#include "private/list_internal.h"
#include "list.h"

list_node_t *list__create_node(void *data)
{
    list_node_t *node = malloc(sizeof(list_node_t));

    if (node == NULL)
        return (NULL);
    node->data = data;
    node->next = node;
    node->prev = node;
    return (node);
}