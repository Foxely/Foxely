/*
** EPITECH PROJECT, 2019
** hash_map
** File description:
** hash map
*/

#include <stdlib.h>
#include "private/hashmap_internal.h"
#include "hashmap.h"

static int
hashmap__foreach(hashmap_pair_t *temp, hashmap_cb *cb, void *data)
{
    int err = 0;

    while (temp && err == 0) {
        err = cb(data, temp->value);
        temp = temp->next;
    }
    return (err);
}

int hashmap_foreach(hashmap_t *hashmap, hashmap_cb *cb, void *data)
{
    int err = 0;
    hashmap_pair_t *temp;

    for (usize_t i = 0; err == 0 && i < hashmap->bucket_count; i++)
        if (hashmap->entries[i]) {
            temp = hashmap->entries[i];
            err = hashmap__foreach(temp, cb, data);
        }
    return (err);
}