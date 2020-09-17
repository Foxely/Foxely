/*
** EPITECH PROJECT, 2019
** json_get
** File description:
** get values from json_t
*/

#include "json.h"

int json_array_get_bool(json_t *root, int index)
{
    return ((json_t *) vector_get(&root->array, index))->boolean;
}

double json_array_get_double(json_t *root, int index)
{
    return ((json_t *) vector_get(&root->array, index))->number;
}

char *json_array_get_string(json_t *root, int index)
{
    return ((json_t *) vector_get(&root->array, index))->string;
}

vector json_array_get_array(json_t *root, int index)
{
    return ((json_t *) vector_get(&root->array, index))->array;
}

vector json_array_get_object(json_t *root, int index)
{
    return ((json_t *) vector_get(&root->array, index))->object;
}