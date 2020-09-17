/*
** EPITECH PROJECT, 2019
** hash_map
** File description:
** hash map
*/

#include <stdlib.h>
#include "private/hashmap_internal.h"
#include "hashmap.h"

hashmap_t *hashmap_new(void)
{
    return (hashmap_new_with_size(32));
}

hashmap_t *hashmap_new_with_size(usize_t size)
{
    hashmap_t *hashmap = malloc(sizeof(hashmap_t));

    if (hashmap == NULL)
        return (NULL);
    hashmap->bucket_count = size;
    hashmap->entries = malloc(hashmap->bucket_count * sizeof(hashmap_pair_t *));
    my_memset(hashmap->entries, 0,
            hashmap->bucket_count * sizeof(hashmap_pair_t *));
    if (hashmap->entries == NULL) {
        free(hashmap);
        return (NULL);
    }
    return (hashmap);
}