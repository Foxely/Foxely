/*
** EPITECH PROJECT, 2019
** EpiProjectGen
** File description:
** main
*/

#include <stdint.h>
#include <stdlib.h>

struct meta {
    void (*dtor)(void *);
    void *ptr;
};

static struct meta *get_meta(void *ptr)
{
    return ptr - sizeof(struct meta);
}

__attribute__((malloc))

void *smalloc(size_t size, void (*dtor)(void *))
{
    struct meta *meta = malloc(sizeof(struct meta) + size);
    *meta = (struct meta) {
        .dtor = dtor,
        .ptr  = meta + 1
    };
    return meta->ptr;
}

void sfree(void *ptr)
{
    if (ptr == NULL)
        return;
    struct meta *meta = get_meta(ptr);
    if (ptr != meta->ptr)
        return;
    if (meta->dtor)
        meta->dtor(ptr);
    free(meta);
}