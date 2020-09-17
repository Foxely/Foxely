/*
** EPITECH PROJECT, 2019
** my_putstr
** File description:
** Function that displays, one-by-one, the characters of a string.
*/

#include <unistd.h>
#include "my.h"

void my_putstr(char const *str)
{
    int len = my_strlen(str);
    write(1, str, len);
}