/*
** EPITECH PROJECT, 2019
** create_element
** File description:
** create a new element
*/

#include <stdlib.h>
#include "list.h"
#include "my.h"
#include "private/list_internal.h"

bool list_insert(list_t *self, usize_t i, void *data)
{
    bool err = 0;
    list_node_t *head = self->head;

    if (i < self->len) {
        self->head = list__get_nth_node(head, i % self->len);
        err = list_push_back(self, data);
        self->head = head;
    }
    return (err);
}