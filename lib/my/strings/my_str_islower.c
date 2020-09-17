/*
** EPITECH PROJECT, 2019
** my_str_islower
** File description:
** Function that verify if the string is lower or not
*/

#include "my.h"

int my_str_islower(char const *str)
{
    if (my_strlen(str) == 0)
        return (1);

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            return (0);
        }
    }
    return (1);
}