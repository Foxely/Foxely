/*
** EPITECH PROJECT, 2019
** my_strstr
** File description:
** Function that display in the index of the keyword
*/

#include "my.h"

static int compare(char const *s1, char const *s2, int i)
{
    for (int j = 0; s1[i] != '\0'; i++, j++) {
        if (s1[i] != s2[j] && s2[j] != '\0')
            return (0);
        if (s2[j] == '\0')
            return (1);
    }
    return (0);
}

char *my_strstr(char *str, char const *to_find)
{
    unsigned int i = 0;

    for (; str[i] != '\0'; i++) {
        if (str[i] == to_find[0] && compare(str, to_find, i)) {
            break;
        }
    }
    for (; i != 0; i--) {
        str++;
    }
    return (str);
}