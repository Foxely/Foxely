/*
** EPITECH PROJECT, 2019
** hash_map
** File description:
** hash map
*/

#include <stdlib.h>
#include "private/hashmap_internal.h"
#include "hashmap.h"

static void
free_hm(hashmap_t *hashmap, hashmap_pair_t *temp, hashmap_pair_t *pair, int bin)
{
    if (pair == hashmap->entries[bin])
        hashmap->entries[bin] = pair->next;
    else
        temp->next = pair->next;
    pair->next = NULL;
    free(pair->key);
    free(pair);
}

void hashmap_remove(hashmap_t *hashmap, char *key)
{
    int bin = ht_hash(hashmap, key);
    hashmap_pair_t *temp, *pair;

    pair = hashmap->entries[bin];
    temp = pair;
    while (pair != NULL) {
        if (my_strcmp(key, pair->key) == 0) {
            my_printf("Data deleted successfully from Hash entries\n");
            free_hm(hashmap, temp, pair, bin);
            return;
        }
        temp = pair;
        pair = pair->next;
    }
    my_printf("Given data is not present in hash entries!!!!\n");
}