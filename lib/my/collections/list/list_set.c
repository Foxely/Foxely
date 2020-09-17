/*
** EPITECH PROJECT, 2019
** list_set_at
** File description:
** replace the old value to the new on the node finded with the index
*/

#include <stdlib.h>
#include "list.h"
#include "private/list_internal.h"

int list_set(list_t *ls, int index, void *item)
{
    if ((usize_t)index >= ls->len)
        return (-1);
    list_node_t *tmp = list__get_nth_node(ls->head, index);
    tmp->data = item;
    return (0);
}