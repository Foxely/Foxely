/*
** EPITECH PROJECT, 2019
** my_str_isalpha
** File description:
** Function that verify that the string have only letters
*/

#include "my.h"

int my_str_isalpha(char const *str)
{
    int len = my_strlen(str);

    if (len == 0)
        return (1);
    for (int i = 0; i < len; i++) {
        if (!my_char_isalpha(str[i])) {
            return (0);
        }
    }
    return (1);
}