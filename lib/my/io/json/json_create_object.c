/*
** EPITECH PROJECT, 2019
** json_create_object
** File description:
** create a object for json
*/

#include <stdlib.h>
#include "garbage.h"
#include "json.h"
#include "my.h"

json_t *json_create_object(char *key)
{
    json_t *object = gc_malloc(get_garbage(), sizeof(json_t));
    object->type = TYPE_OBJECT;
    vector_init(&object->object, 1);
    if (key)
        object->key = my_strdup(key);
    return object;
}