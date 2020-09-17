/*
** EPITECH PROJECT, 2019
** json_get
** File description:
** get values from json_t
*/

#include "json.h"

json_t *json_array_get_raw(json_t *root, int index)
{
    json_t *res = vector_get(&root->array, index);
    return (res ? res : 0);
}