/*
** EPITECH PROJECT, 2019
** new_list
** File description:
** create a new list
*/

#include "list.h"
#include "my.h"
#include "private/list_internal.h"

list_t *new_list(void)
{
    list_t *ls = malloc(sizeof(list_t));

    if (ls) {
        ls->head = NULL;
        ls->len = 0;
        return (ls);
    }
    return (NULL);
}