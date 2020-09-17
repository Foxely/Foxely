/*
** EPITECH PROJECT, 2019
** my_strdup
** File description:
** Func that allocates memory and copies the string
*/

#include <stdlib.h>
#include "my.h"

char *my_strdup(char const *src)
{
    char *str;
    int len = my_strlen(src) + 1;
    str = malloc(sizeof(char) * len);
    my_strcpy(str, src);
    return (str);
}