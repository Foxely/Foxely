/*
** EPITECH PROJECT, 2019
** json_get
** File description:
** get values from json_t
*/

#include "json.h"

json_t *json_object_get_raw(json_t *root, char *key)
{
    json_t *res = json_value_with_key(root, key);
    return (res ? res : 0);
}