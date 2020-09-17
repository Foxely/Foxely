/*
** EPITECH PROJECT, 2019
** hash_map
** File description:
** hash map
*/

#include <stdlib.h>
#include "private/hashmap_internal.h"
#include "hashmap.h"

u64_t ht_hash(hashmap_t *hm, const char *str)
{
    u64_t hash = 5381;

    for (usize_t i = 0; str[i]; i++)
        hash += (hash << 5) + str[i];
    return (hash % hm->bucket_count);
}