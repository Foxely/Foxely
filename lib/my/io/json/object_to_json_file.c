/*
** EPITECH PROJECT, 2019
** object_to_json_file
** File description:
** convert an json object to a json file
*/

#include "json.h"
#include "my.h"
#include "vector.h"

static int construct_array(int fd, json_t *json)
{
    json_t *temp;

    for (size_t i = 0; i < json->array.size - 1; i++) {
        temp = vector_get(&json->array, i);
        fsprintf(fd, "\"%s\",", temp->string);
    }
    temp = vector_get(&json->array, json->array.size - 1);
    fsprintf(fd, "\"%s\"", temp->string);
    return (0);
}

int object_to_json_file(int fd, json_t *json)
{
    if (json->type == TYPE_STRING) {
        if (json->key)
            fsprintf(fd, "\"%s\":", json->key);
        fsprintf(fd, "\"%s\",\n", json->string);
    }
    if (json->type == TYPE_ARRAY) {
        fsprintf(fd, "\"%s\":[", json->key);
        construct_array(fd, json);
        fsprintf(fd, "],\n");
    }
    if (json->type == TYPE_OBJECT) {
        if (json->key)
            fsprintf(fd, "\"%s\":", json->key);
        fsprintf(fd, "{\n");
        for (size_t i = 0; i < json->object.size; i++)
            object_to_json_file(fd, json->object.data[i]);
        fsprintf(fd, "}\n");
    }
    return 1;
}