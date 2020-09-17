/*
** EPITECH PROJECT, 2019
** vector
** File description:
** Header contains all functions prototyped
*/

#ifndef _VECTOR_
#define _VECTOR_

#include <stdio.h>

typedef struct s_vector vector;

int vector_destroy(vector *vector);
vector *vector_add(vector **head_ref, void *data);
void *vector_get(vector *vector, size_t index);
int vector_getsize(vector *begin);
void vector_foreach(vector *v, int (*callback)(void *, void *), void *data);
int vector_is_initialized(vector *vector);
int vector_copy(vector *dest, vector *src);
int vector_init(vector *vector, size_t capacity);
int vector_push_front(vector *vector, void *element);
int vector_push_back(vector *vector, void *element);
int vector_pop_back(vector *vector);
int vector_pop_front(vector *vector);
int vector_assign(vector *vector, size_t index, void *element);
int vector_move_assign(vector *destination, vector *source);
int vector_move(vector *destination, vector *source);
int vector_swap(vector *destination, vector *source);
int vector_insert(vector *vector, size_t index, void *element);
void *vector_front(vector *vector);
void *vector_back(vector *vector);
int vector_resize(vector *vector, size_t new_size);
int vector_remove(vector *vector, size_t index);
int vector_clear(vector *vector);
int vector_reserve(vector *vector, size_t minimum_capacity);
int vector_is_empty(const vector *vector);
size_t vector_free_space(const vector *vector);

/****************************************************************************/

#endif