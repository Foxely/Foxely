/*
** EPITECH PROJECT, 2019
** json_parse_object
** File description:
** json parse obj
*/

#include <stdlib.h>
#include "garbage.h"
#include "json.h"
#include "my.h"
#include "vector.h"

json_t *json_create(void)
{
    json_t *temp = gc_malloc(get_garbage(), sizeof(json_t));
    temp->type = TYPE_NULL;
    temp->key = NULL;
    return temp;
}

bool check(char **cursor, json_t *result, int *success, json_t *value)
{
    if (*success) {
        vector_push_back(&result->object, value);
    } else {
        json_free_value(NULL, value);
        return (true);
    }
    if (has_char(cursor, '}'))
        return (true);
    else if (has_char(cursor, ','))
        return (false);
    else
        success = 0;
    return (false);
}

int json_parse_object(char **cursor, json_t *parent)
{
    json_t result = {.key = parent->key, .type = TYPE_OBJECT };
    int success = 1;

    vector_init(&result.object, 1);
    while (success && !has_char(cursor, '}')) {
        json_t *temp = json_create(), *value = json_create();
        success = json_parse_value(cursor, temp);
        if (temp->type != TYPE_STRING) {
            my_memcpy(value, temp, sizeof(json_t));
        } else {
            value->key = my_strdup(temp->string);
            json_free_value(NULL, temp);
            success = success && has_char(cursor, ':');
            success = success && json_parse_value(cursor, value);
        }
        if (check(cursor, &result, &success, value))
            break;
    }
    (success) ? (*parent = result), 0 : json_free_value(NULL, &result);
    return success;
}

int json_parse_string(char **cursor, json_t *parent)
{
    ++(*cursor);
    const char *start = *cursor;
    char *end = my_strchr(*cursor, '"');
    if (end) {
        size_t len = end - start;
        char *new_string = gc_malloc(get_garbage(), (len + 1) * sizeof(char));
        my_memcpy(new_string, start, len);
        new_string[len] = '\0';
        parent->type = TYPE_STRING;
        parent->string = new_string;
        *cursor = end + 1;
        return 1;
    }
    return 0;
}