/*
** EPITECH PROJECT, 2019
** my_strcpy
** File description:
** Function that copies a string into another
*/

#include "my.h"

int my_strcmp(char const *s1, char const *s2)
{
    for (int i = 0; s1[i] || s2[i]; i++)
        if (s1[i] != s2[i])
            return ((int) s1[i] - s2[i]);
    return (0);
}