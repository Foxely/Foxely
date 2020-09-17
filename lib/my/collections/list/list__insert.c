/*
** EPITECH PROJECT, 2019
** list_insert
** File description:
** insert a node in a list
*/

#include "private/list_internal.h"

void list__insert_after(list_node_t *self, list_node_t *other)
{
    other->next = self->next;
    other->prev = self;
    other->prev->next = other;
    other->next->prev = other;
}

void list__insert_before(list_node_t *self, list_node_t *other)
{
    other->next = self;
    other->prev = self->prev;
    other->prev->next = other;
    other->next->prev = other;
}