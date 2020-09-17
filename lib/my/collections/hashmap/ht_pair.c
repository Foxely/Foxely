/*
** EPITECH PROJECT, 2019
** hash_map
** File description:
** hash map
*/

#include <stdlib.h>
#include "hashmap.h"
#include "private/hashmap_internal.h"

hashmap_pair_t *ht_pair(const char *key, void *value)
{
    hashmap_pair_t *entry = malloc(sizeof(hashmap_pair_t));
    hashmap_pair_t init = {.key = my_strdup(key), .value = value};

    if (!entry) {
        free(init.key);
        return (NULL);
    }
    my_memcpy(entry, &init, sizeof(hashmap_pair_t));
    entry->next = NULL;
    return (entry);
}