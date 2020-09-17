/*
** EPITECH PROJECT, 2019
** my_str_isupper
** File description:
** Function that verify if the string is upper or not
*/

#include "my.h"

int my_str_isupper(char const *str)
{
    if (my_strlen(str) == 0)
        return (1);
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            return (0);
        }
    }
    return (1);
}