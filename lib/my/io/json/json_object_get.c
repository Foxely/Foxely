/*
** EPITECH PROJECT, 2019
** json_get
** File description:
** get values from json_t
*/

#include "json.h"

int json_object_get_bool(json_t *root, char *key)
{
    json_t *res = json_value_with_key(root, key);
    return (res ? res->boolean : 84);
}

double json_object_get_double(json_t *root, char *key)
{
    json_t *res = json_value_with_key(root, key);
    return (res ? res->number : 84);
}

char *json_object_get_string(json_t *root, char *key)
{
    json_t *j = json_value_with_key(root, key);
    return j == NULL ? NULL : j->string;
}

vector json_object_get_array(json_t *root, char *key)
{
    json_t *res = json_value_with_key(root, key);
    return (res ? res->array : (vector){0});
}

vector json_object_get_object(json_t *root, char *key)
{
    json_t *res = json_value_with_key(root, key);
    return (res ? res->object : (vector){0});
}