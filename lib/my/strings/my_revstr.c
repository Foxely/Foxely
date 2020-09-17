/*
** EPITECH PROJECT, 2019
** my_revstr
** File description:
** Function that reverse the string passed
*/

#include "my.h"

static void my_swap_character(char *a, char *b)
{
    char c = *a;
    *a = *b;
    *b = c;
}

char *my_revstr(char *str)
{
    int len = my_strlen(str) - 1;

    for (int i = 0; i < len; i++, len--) {
        my_swap_character(&str[i], &str[len]);
    }
    return (str);
}