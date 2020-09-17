/*
** EPITECH PROJECT, 2019
** json_parse_value
** File description:
** json value parser
*/

#include <stdlib.h>
#include "json.h"
#include "my.h"

static void litteral_condition(char **cursor, json_t *parent, int *success)
{
    if (**cursor == 't') {
        *success = json_is_literal(cursor, "true");
        if (*success) {
            parent->type = TYPE_BOOL;
            parent->boolean = 1;
        }
    }
    if (**cursor == 'f') {
        *success = json_is_literal(cursor, "false");
        if (*success) {
            parent->type = TYPE_BOOL;
            parent->boolean = 0;
        }
    }
    if (**cursor == 'n')
        *success = json_is_literal(cursor, "null");
}

static void array_and_object_condition(char **cursor,
                                        json_t *parent, int *success)
{
    if (**cursor == '{') {
        ++(*cursor);
        *success = json_parse_object(cursor, parent);
    }
    if (**cursor == '[') {
        parent->type = TYPE_ARRAY;
        vector_init(&parent->array, 1);
        ++(*cursor);
        *success = json_parse_array(cursor, parent);
        if (!*success) {
            json_free_value(NULL, parent);
        }
    }
}

static void number_condition(char **cursor, json_t *parent, int *success)
{
    if (my_char_isnum(**cursor) || **cursor == '-') {
        char *end;
        double number = my_strtod(*cursor, &end);
        if (**cursor != *end) {
            parent->type = TYPE_NUMBER;
            parent->number = number;
            *cursor = end;
            *success = 1;
        }
    }
}

int json_parse_value(char **cursor, json_t *parent)
{
    int success = 1;

    if (**cursor == '\0') {
        success = 0;
        return success;
    }
    if (**cursor == '"')
        json_parse_string(cursor, parent);
    array_and_object_condition(cursor, parent, &success);
    litteral_condition(cursor, parent, &success);
    number_condition(cursor, parent, &success);
    return success;
}