/*
** EPITECH PROJECT, 2019
** my_printf
** File description:
** The Ultime Printf
*/

#include "my.h"

void my_putbase(int num, const char *base)
{
    int base_size = my_strlen(base);

    if (num / base_size > 0)
        my_putbase(num / base_size, base);

    my_putchar(base[num % base_size]);
}