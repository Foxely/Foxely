/*
** EPITECH PROJECT, 2019
** my_str_clean
** File description:
** clean a string
*/

#include <stdlib.h>
#include "my.h"

void my_str_clean(char *str)
{
    char *temp = malloc(sizeof(char) * my_strlen(str) + 1);
    my_strcpy(temp, str);
    my_memset(str, 0, my_strlen(str));

    for (int i = 0, j = 0; temp[i]; i++) {
        if (temp[i] == '\t' || temp[i] == ' ' || temp[i] == '\n')
            continue;
        str[j++] = temp[i];
    }
    free(temp);
}