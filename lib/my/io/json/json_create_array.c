/*
** EPITECH PROJECT, 2019
** json_create_array
** File description:
** create a array for json
*/

#include <stdlib.h>
#include <stdarg.h>
#include "json.h"

json_t json_create_array(char *name, int count, ...)
{
    va_list ap;
    json_t array;

    va_start(ap, count);
    array.type = TYPE_ARRAY;
    array.key = name;
    vector_init(&array.array, 1);
    for (int i = 0; i < count; i++) {
        vector_push_back(&array.array,
                        json_create_string(NULL, va_arg(ap, char *)));
    }
    return (array);
}