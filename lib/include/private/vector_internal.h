/*
** EPITECH PROJECT, 2019
** vector
** File description:
** Header contains all functions prototyped
*/

#ifndef vector__PRIVATE_
#define vector__PRIVATE_

#include <stdio.h>

/********************************STRUCT***********************************/

typedef struct s_vector
{
    size_t size;
    size_t capacity;
    size_t element_size;
    void **data;
} vector;

/****************************************************************************/

#define VECTOR_MINIMUM_CAPACITY (2)
#define VECTOR_GROW_FACT (2)
#define VECTOR_SHRINK_THRESHOLD (1 / 4)
#define MAX(a, b) ((a) > (b) ? (a) : (b))

size_t vector_byte_size(const vector *vector);
int vector__should_grow(vector *vector);
int vector__reallocate(vector *vector, size_t new_capacity);
int vector__adjust_capacity(vector *vector);
void vector__assign(vector *vector, size_t index, void *element);
void *vector__offset(vector *vector, size_t index);
int vector__move_right(vector *vector, size_t index);
void vector__move_left(vector *vector, size_t index);
void vector__swap(size_t *first, size_t *second);
int vector__should_shrink(vector *vector);
int vector_shrink_to_fit(vector *vector);

/****************************************************************************/

#endif