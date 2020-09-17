/*
** EPITECH PROJECT, 2019
** hashmap_insert
** File description:
** insert a element
*/

#include <stdlib.h>
#include "private/hashmap_internal.h"
#include "hashmap.h"

void hashmap_insert(hashmap_t *hashmap, const char *key, void *value)
{
    unsigned int slot = ht_hash(hashmap, key);
    hashmap_pair_t *entry = hashmap->entries[slot];
    hashmap_pair_t *prev;

    if (entry == NULL) {
        hashmap->entries[slot] = ht_pair(key, value);
        return;
    }
    while (entry != NULL) {
        if (my_strcmp(entry->key, key) == 0) {
            entry->value = value;
            return;
        }
        prev = entry;
        entry = prev->next;
    }
    prev->next = ht_pair(key, value);
}