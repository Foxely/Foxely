/*
** EPITECH PROJECT, 2019
** dictionnary
** File description:
** Header contains all functions prototyped
*/

#ifndef _MY_HASHMAP_
#define _MY_HASHMAP_

#include <stdio.h>
#include "my.h"
#include "json.h"

typedef int (hashmap_cb)(void *, void *);
typedef struct hashmap hashmap_t;

/*************************HASHMAP FUNCTIONS*******************************/

hashmap_t *hashmap_new(void);
hashmap_t *hashmap_new_with_size(usize_t size);
void hashmap_insert(hashmap_t *hashmap, const char *key, void *value);
void hashmap_remove(hashmap_t *hashmap, char *key);
void *hashmap_get(hashmap_t *hashmap, const char *key);
int hashmap_foreach(hashmap_t *hashmap, hashmap_cb *cb, void *data);
void hashmap_destroy_with(hashmap_t *hashmap, hashmap_cb *cb, void *user_data);
void hashmap_destroy(hashmap_t *self);
void hashmap_print(hashmap_t *hashtable);

hashmap_t *hashmap_from_json(char *json_file, void *(*create)(json_t *));

/****************************************************************************/

#endif