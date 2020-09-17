/*
** EPITECH PROJECT, 2019
** my_charcat
** File description:
** Function that concatenates two strings
*/

#include "my.h"

char *my_charcat(char *dest, char src)
{
    int dest_len = my_strlen(dest);

    dest[dest_len] = src;
    dest[dest_len + 1] = '\0';
    return dest;
}