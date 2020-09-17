/*
** EPITECH PROJECT, 2019
** json_parser
** File description:
** parser for json
*/

#include "garbage.h"
#include "json.h"
#include "my.h"

int has_char(char **cursor, char character)
{
    int success = **cursor == character;
    if (success)
        ++(*cursor);
    return success;
}

int json_parse_array(char **cursor, json_t *parent)
{
    int success = 1;

    if (**cursor == ']') {
        ++(*cursor);
        return success;
    }
    while (success) {
        json_t *new_value = gc_malloc(get_garbage(), sizeof(json_t));
        new_value->type = TYPE_NULL;
        success = json_parse_value(cursor, new_value);
        if (!success)
            break;
        vector_push_back(&parent->array, new_value);
        if (has_char(cursor, ']'))
            break;
        else if (has_char(cursor, ','))
            continue;
        else success = 0;
    }
    return success;
}

int json_free_value(void *data, json_t *val)
{
    (void) data;
    if (!val)
        return (0);
    if (val->type == TYPE_STRING) {
        gc_free(get_garbage(), val->string);
        val->string = NULL;
    }
    if (val->type == TYPE_ARRAY)
        vector_destroy(&val->array);
    if (val->type == TYPE_OBJECT) {
        vector_foreach(&(val->object), json_free_value, NULL);
        vector_destroy(&val->object);
    }
    gc_free(get_garbage(), val);
    val->type = TYPE_NULL;
    return 1;
}

int json_is_literal(char **cursor, char *literal)
{
    size_t cnt = my_strlen(literal);

    if (my_strncmp(*cursor, literal, cnt) == 0) {
        *cursor += cnt;
        return 1;
    }
    return 0;
}

int json_parse(char *input, json_t *result)
{
    return json_parse_value(&input, result);
}