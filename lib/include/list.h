/*
** EPITECH PROJECT, 2019
** list_t
** File description:
** Header contains all functions prototyped
*/

#ifndef _MY_LIST_
#define _MY_LIST_

#include <stdio.h>
#include "my.h"

#define LIST_FOR_EACH_COND(self, name, cond) \
    for (list_iter_t name = list_iter(self); \
        !list_iter_end(&name) && (cond); list_iter_next(&name))

#define LIST_FOR_EACH(self, name) LIST_FOR_EACH_COND(self, name, true)

typedef int (list_iter_fn_t)(void *, void *);
typedef struct s_list_node list_node_t;
typedef struct s_list list_t;
typedef struct s_list_iter list_iter_t;

/******************************LIST FUNCTIONS*******************************/

list_t *new_list(void);
void *list_get(const list_t *ls, usize_t i);
int list_set(list_t *ls, int index, void *item);
int list_foreach(list_t *v, int (*callback)(void *, void *), void *data);
bool list_insert(list_t *self, usize_t i, void *data);
void list_destroy(list_t *self);
void list_destroy_with(list_t *, list_iter_fn_t *destroyer, void *user_data);
void list_clear(list_t *self);
void list_clear_with(list_t *self, list_iter_fn_t *destroyer, void *data);
void *list_find(list_t *self, void *element);
void *list_find_with(list_t *self, list_iter_fn_t *fn, void *element);
list_t *list_from(usize_t count, ...);
list_t *list_from_var(usize_t count, va_list ap);
list_t *list_from_arr(usize_t count, void **data);
void *list_find(list_t *self, void *element);
void *list_find_with(list_t *self, list_iter_fn_t *fn, void *element);
void *list_pop_front(list_t *self);
void *list_pop_back(list_t *self);
bool list_push_front(list_t *self, void *data);
bool list_push_back(list_t *self, void *data);
void *list_remove(list_t *self, usize_t i);

list_iter_t list_iter(const list_t *self);
bool list_iter_end(const list_iter_t *iter);
void list_iter_next(list_iter_t *iter);

/****************************************************************************/

#endif