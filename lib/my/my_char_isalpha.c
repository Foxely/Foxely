/*
** EPITECH PROJECT, 2019
** my_char_isalpha
** File description:
** Function that verify that the char is only letters
*/

#include "my.h"

int my_char_isalpha(char str)
{
    if (str >= 'a' && str <= 'z') {
        return (1);
    } else if (str >= 'A' && str <= 'Z') {
        return (1);
    }
    return (0);
}