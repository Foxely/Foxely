/*
** EPITECH PROJECT, 2019
** list_get
** File description:
** get the data with the index we given
*/

#include "list.h"
#include "my.h"
#include "private/list_internal.h"

void *list_get(const list_t *ls, usize_t i)
{
    if (i >= ls->len)
        return (NULL);
    return (list__get_nth_node(ls->head, i)->data);
}