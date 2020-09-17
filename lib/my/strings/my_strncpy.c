/*
** EPITECH PROJECT, 2019
** my_strncpy
** File description:
** Function that copies a string into another with n character
*/

#include "my.h"

char *my_strncpy(char *dest, char const *src, int n)
{
    int i;
    int len = my_strlen(src);

    for (i = 0; i < n; i++)
        dest[i] = src[i];
    if (n >= len)
        dest[i] = '\0';
    return dest;
}