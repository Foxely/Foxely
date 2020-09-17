/*
** EPITECH PROJECT, 2019
** dictionnary
** File description:
** Header contains all functions prototyped
*/

#ifndef _MY_HASHMAP_PRIVATE_
#define _MY_HASHMAP_PRIVATE_

#include <stdio.h>
#include "my.h"

typedef int (hashmap_cb)(void *, void *);

/********************************STRUCT***********************************/

typedef struct hashmap_pair {
    char *key;
    void *value;
    struct hashmap_pair *next;
} hashmap_pair_t;

typedef struct hashmap {
    usize_t size;
    usize_t bucket_count;
    hashmap_pair_t **entries;
} hashmap_t;

/****************************************************************************/
/*************************HASHMAP FUNCTIONS*******************************/

hashmap_pair_t *ht_pair(const char *key, void *value);
u64_t ht_hash(hashmap_t *hm, const char *str);

/****************************************************************************/

#endif