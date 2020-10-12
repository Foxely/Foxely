/*
** EPITECH PROJECT, 2019
** my_event
** File description:
** Header Event Container
*/

#ifndef _GARBAGE_
#define _GARBAGE_

#include <stdlib.h>

#define STACK_MAX (256)

typedef enum t {
    OBJ_INT,
    OBJ_VOID_PTR,
    OBJ_PAIR
} object_type;

typedef struct s_object {
    object_type type;
    unsigned char marked;
    size_t hash;
    int id;
    struct s_object *next;

    union {
        int value;
        void *data;
    };
} object_t;

typedef struct {
    object_t *stack[STACK_MAX];
    int stack_size;
    object_t *first_object;
    int num_objects;
    int max_objects;
    int object_collected;
} gc_t;

////////////////////////////////////////////////////////

void gc_run(gc_t *vm);
void gc_stop(gc_t *vm);
gc_t *my_gc_new(void);
object_t *new_object(gc_t *vm, object_type type, size_t size);
void *gc_malloc(gc_t *vm, size_t size);
void gc_free(gc_t *vm, void *ptr);
gc_t *get_garbage(void);
int assert(int condition, const char *message);
void mark_all(gc_t *vm);
void gc_push(gc_t *vm, object_t *value);
void sweep(gc_t *vm);

#endif