/*
** EPITECH PROJECT, 2019
** list_t
** File description:
** Header contains all functions prototyped
*/

#ifndef _MY_LIST_PRIVATE_
#define _MY_LIST_PRIVATE_

#include <stdio.h>
#include "my.h"

/********************************STRUCT***********************************/

typedef struct s_list_node
{
    void *data;
    struct s_list_node *prev;
    struct s_list_node *next;
} list_node_t;

typedef struct s_list
{
    list_node_t *head;
    usize_t len;
} list_t;

typedef struct s_list_iter
{
    const list_node_t *next;
    const list_t *list;
    usize_t i;
    void *v;
} list_iter_t;

/****************************************************************************/
/******************************LIST FUNCTIONS*******************************/

list_node_t *list__get_nth_node(list_node_t *v, int index);
void *list__destroy_node(list_node_t *node);
void list__insert_after(list_node_t *self, list_node_t *other);
void list__insert_before(list_node_t *self, list_node_t *other);
list_node_t *list__create_node(void *data);

/****************************************************************************/

#endif