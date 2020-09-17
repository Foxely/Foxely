/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#ifndef __MEM_H__
#define __MEM_H__

#include "object.h"

#define GC_HEAP_GROW_FACTOR (2)

#define ALLOCATE(vm, type, count) \
    (type *)reallocate(vm, NULL, 0, sizeof(type) * (count))

#define FREE(vm, type, pointer) \
    reallocate(vm, pointer, sizeof(type), 0)

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(vm, previous, type, old_count, count) \
(type *)reallocate(vm, previous,                    \
                sizeof(type) * (old_count), sizeof(type) * (count))

#define FREE_ARRAY(vm, type, pointer, oldCount) \
    reallocate(vm, pointer, sizeof(type) * (oldCount), 0)


void *reallocate(vm_t *vm, void *previous, size_t old_size, size_t new_size);
void mark_object(vm_t *vm, obj_t *object);
void mark_value(vm_t *vm, value value);
void free_objects(vm_t *vm);
void collect_garbage(vm_t *vm);
void trace_references(vm_t *vm);

#endif