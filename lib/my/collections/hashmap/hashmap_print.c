/*
** EPITECH PROJECT, 2019
** hash_map
** File description:
** hash map
*/

#include <stdlib.h>
#include "private/hashmap_internal.h"
#include "hashmap.h"

void hashmap_print(hashmap_t *hashtable)
{
    for (usize_t i = 0; i < hashtable->bucket_count; ++i) {
        hashmap_pair_t *entry = hashtable->entries[i];
        my_printf("slot[%d]: ", i);
        if (entry == NULL) {
            my_printf("****************\n");
            continue;
        }
        for (;entry; entry = entry->next) {
            my_printf("%s ", entry->key);
        }
        my_printf("\n");
    }
}