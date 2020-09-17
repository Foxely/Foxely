/*
** EPITECH PROJECT, 2019
** json_get
** File description:
** get values from json_t
*/

#include "json.h"

char *json_to_string(json_t *value)
{
    return (char *)value->string;
}

double json_to_double(json_t *value)
{
    return value->number;
}

int json_to_bool(json_t *value)
{
    return value->boolean;
}

vector *json_to_array(json_t *value)
{
    return &value->array;
}

vector *json_to_object(json_t *value)
{
    return &value->object;
}