/*
** EPITECH PROJECT, 2019
** list_remove
** File description:
** list remove
*/

#include "list.h"
#include "my.h"
#include "private/list_internal.h"

void *list_remove(list_t *self, usize_t i)
{
    void *data = NULL;
    list_node_t *head = self->head;

    if (i >= self->len)
        return (NULL);
    self->head = list__get_nth_node(head, i);
    if (self->head == head) {
        if (head->next == head)
            head = NULL;
        else
            head = head->next;
    }
    data = list_pop_front(self);
    self->head = head;
    return (data);
}