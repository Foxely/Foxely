/*
** EPITECH PROJECT, 2019
** json_get
** File description:
** get values from json_t
*/

#include "json.h"
#include "my.h"

json_t *json_value_with_key(json_t *root, char *key)
{
    json_t **data = (json_t **)root->object.data;
    size_t size = root->object.size;

    for (size_t i = 0; i < size; i++) {
        if (data[i]->key && my_strcmp(data[i]->key, key) == 0)
            return data[i];
    }
    return NULL;
}