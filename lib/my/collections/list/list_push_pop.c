/*
** EPITECH PROJECT, 2019
** list_push_pop
** File description:
** push pop node
*/

#include "list.h"
#include "private/list_internal.h"

void *list_pop_front(list_t *self)
{
    void *data = NULL;
    list_node_t *head = self->head;

    if (self->head) {
        self->head = head->next;
        data = list_pop_back(self);
        return (data);
    }
    return (NULL);
}

void *list_pop_back(list_t *self)
{
    void *data = NULL;
    list_node_t *head = self->head;

    if (self->head) {
        data = list__destroy_node(head->prev);
        self->len--;
        if (self->len == 0)
            self->head = NULL;
        return (data);
    }
    return (NULL);
}

bool list_push_front(list_t *self, void *data)
{
    list_node_t *node = list__create_node(data);

    if (node) {
        if (self->head)
            list__insert_before(self->head, node);
        self->head = node;
        self->len++;
        return (true);
    }
    return (false);
}

bool list_push_back(list_t *self, void *data)
{
    if (!list_push_front(self, data))
        return (false);
    self->head = self->head->next;
    return (true);
}