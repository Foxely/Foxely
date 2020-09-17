/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#include <stdlib.h>
#include "custom_memory.h"
#include "vm.h"

void *reallocate(vm_t *vm, void *previous, size_t old_size, size_t new_size)
{
    vm->bytes_allocated += new_size - old_size;
    if (new_size > old_size) {
#ifdef DEBUG_STRESS_GC
    collect_garbage(vm);
#endif
        if (vm->bytes_allocated > vm->next_gc) {
            collect_garbage(vm);
        }
    }
    if (new_size == 0) {
        free(previous);
        return NULL;
    }
    return realloc(previous, new_size);
}