/*
** EPITECH PROJECT, 2019
** my_strcapitalize
** File description:
** Function that reverse the string passed
*/

#include "my.h"

char *my_strcapitalize(char *str)
{
    str = my_strlowcase(str);
    if (str[0] >= 'a' && str[0] <= 'z') {
        str[0] -= 32;
    }
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            if (str[i - 1] == ' ' || str[i - 1] == '-')
                str[i] -= 32;
            else if (str[i - 1] == '+') {
                str[i] -= 32;
            }
        }
    }
    return (str);
}