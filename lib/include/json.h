/*
** EPITECH PROJECT, 2020
** json
** File description:
** json CONTAINER
*/

#ifndef _MY_JSON_
#define _MY_JSON_

#include "vector.h"
#include "private/vector_internal.h"

typedef enum json_type {
    TYPE_NULL,
    TYPE_BOOL,
    TYPE_NUMBER,
    TYPE_OBJECT,
    TYPE_ARRAY,
    TYPE_STRING,
} json_type;

typedef struct {
    json_type type;
    char *key;
    union {
        int boolean;
        double number;
        char *string;
        vector array;
        vector object;
    };
} json_t;

int has_char(char **cursor, char character);
int json_parse_object(char **cursor, json_t *parent);
int json_parse_array(char **cursor, json_t *parent);
int json_free_value(void *data, json_t *val);
int json_is_literal(char **cursor, char *literal);
int json_parse(char *input, json_t *result);
int json_parse_string(char **cursor, json_t *parent);
int json_parse_value(char **cursor, json_t *parent);
json_t *json_value_with_key(json_t *root, char *key);

int json_parse_file(char *filepath, json_t *result);
int json_object_get_bool(json_t *root, char *key);
double json_object_get_double(json_t *root, char *key);
char *json_object_get_string(json_t *root, char *key);
vector json_object_get_array(json_t *root, char *key);
vector json_object_get_object(json_t *root, char *key);
json_t *json_object_get_raw(json_t *root, char *key);

int json_array_get_bool(json_t *root, int index);
double json_array_get_double(json_t *root, int index);
char *json_array_get_string(json_t *root, int index);
vector json_array_get_array(json_t *root, int index);
vector json_array_get_object(json_t *root, int index);
json_t *json_array_get_raw(json_t *root, int index);

double my_strtod(char *str, char **endptr);
json_t *json_create_string(char *key, char *value);
int object_to_json_file(int fd, json_t *json);
json_t json_create_array(char *name, int count, ...);
json_t *json_create_object(char *key);

#endif