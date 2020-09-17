/*
** EPITECH PROJECT, 2019
** hash_map
** File description:
** hash map
*/

#include <stdlib.h>
#include "private/hashmap_internal.h"
#include "hashmap.h"
#include "garbage.h"

hashmap_t *hashmap_from_json(char *json_file, void *(*create)(json_t *))
{
    json_t result = { .type = TYPE_NULL };
    char *buffer = load_file_in_mem(json_file);
    hashmap_t *hmap = hashmap_new();

    my_str_clean(buffer);
    json_parse(buffer, &result);
    for (size_t i = 0; i < result.object.size; i++) {
        json_t *obj = vector_get(&result.object, i);
        hashmap_insert(hmap, obj->key, create(obj));
    }
    gc_free(get_garbage(), buffer);
    json_free_value(NULL, &result);
    gc_clear(get_garbage());
    return hmap;
}