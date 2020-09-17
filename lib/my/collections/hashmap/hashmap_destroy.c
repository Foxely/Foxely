/*
** EPITECH PROJECT, 2019
** hash_map
** File description:
** hash map
*/

#include <stdlib.h>
#include "private/hashmap_internal.h"
#include "hashmap.h"

static void destroy_entry(hashmap_pair_t *entry)
{
    hashmap_pair_t *pair = entry;
    hashmap_pair_t *temp = NULL;

    if (entry == NULL)
        return;
    while (pair != NULL) {
        temp = pair->next;
        free(pair->key);
        free(pair);
        pair = temp;
    }
}

void hashmap_destroy(hashmap_t *self)
{
    if (self == NULL)
        return;
    if (self->entries) {
        for (usize_t i = 0; i < self->bucket_count; i++)
            destroy_entry(self->entries[i]);
        free(self->entries);
    }
    free(self);
}

void hashmap_destroy_with(hashmap_t *hashmap, hashmap_cb *cb, void *user_data)
{
    if (cb != NULL)
        hashmap_foreach(hashmap, cb, user_data);
    hashmap_destroy(hashmap);
}