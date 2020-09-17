/*
** EPITECH PROJECT, 2019
** my_realloc
** File description:
** Realloc Memory
*/

#include <stdlib.h>
#include "my.h"

void *my_realloc(void *ptr, size_t original_size, size_t size)
{
    if (size == 0) {
        free(ptr);
        return NULL;
    } else if (!ptr) {
        return malloc(size);
    } else {
        void *ptr_new = malloc(size);
        if (ptr_new) {
            my_memcpy(ptr_new, ptr, original_size);
            free(ptr);
        }
        return ptr_new;
    }
}