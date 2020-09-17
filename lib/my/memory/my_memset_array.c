/*
** EPITECH PROJECT, 2019
** my_str_to_word_array
** File description:
** Func that splits a string into words
*/

#include <stdlib.h>
#include "my.h"

char **my_memset_array(char **ptr, int x, int r, int c)
{
    for (int i = 0; i < r; i++)
        for (int j = 0; j < c; j++)
            ptr[i][j] = x;
    return (ptr);
}