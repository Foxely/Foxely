/*
** EPITECH PROJECT, 2019
** json_parse_file
** File description:
** parse a json with the filename given
*/

#include "my.h"
#include "garbage.h"
#include "json.h"

int json_parse_file(char *filepath, json_t *result)
{
    char *buffer = load_file_in_mem(filepath);
    int err = 0;

    my_str_clean(buffer);
    err = json_parse(buffer, result);
    gc_free(get_garbage(), buffer);
    return (err);
}