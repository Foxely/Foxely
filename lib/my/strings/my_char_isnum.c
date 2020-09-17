/*
** EPITECH PROJECT, 2019
** my_char_isnum
** File description:
** Function that verify that the char is only number
*/

#include "my.h"

int my_char_isnum(char str)
{
    if (str >= '0' && str <= '9')
        return (1);
    return (0);
}