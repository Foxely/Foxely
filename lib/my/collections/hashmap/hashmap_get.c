/*
** EPITECH PROJECT, 2019
** hash_map
** File description:
** hash map
*/

#include <stdlib.h>
#include "private/hashmap_internal.h"
#include "hashmap.h"

void *hashmap_get(hashmap_t *hashmap, const char *key)
{
    unsigned int slot = ht_hash(hashmap, key);
    hashmap_pair_t *entry = hashmap->entries[slot];

    if (entry == NULL) {
        return NULL;
    }
    while (entry != NULL) {
        if (my_strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}