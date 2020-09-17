/*
** EPITECH PROJECT, 2019
** my_str_isnum
** File description:
** Function that verify that the string have only number
*/

#include "my.h"

int my_str_isnum(char const *str)
{
    if (my_strlen(str) == 0)
        return (1);
    for (int i = 0; str[i] != '\0'; i++) {
        if (!my_char_isnum(str[i]) && str[i] != '-')
            return (0);
    }
    return (1);
}