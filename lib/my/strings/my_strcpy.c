/*
** EPITECH PROJECT, 2019
** my_strcpy
** File description:
** Function that copies a string into another
*/

#include "my.h"

char *my_strcpy(char *dest, char const *src)
{
    unsigned int i;

    for (i = 0; src[i] != '\0'; i++)
        dest[i] = src[i];
    dest[i] = '\0';
    return dest;
}