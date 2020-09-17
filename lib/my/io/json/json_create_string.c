/*
** EPITECH PROJECT, 2019
** json_create_string
** File description:
** json create string
*/

#include <stdlib.h>
#include "json.h"
#include "my.h"

json_t *json_create_string(char *key, char *value)
{
    json_t *str = malloc(sizeof(json_t));

    str->type = TYPE_STRING;
    if (key != NULL)
        str->key = my_strdup(key);
    str->string = my_strdup(value);
    return (str);
}