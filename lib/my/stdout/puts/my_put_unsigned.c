/*
** EPITECH PROJECT, 2019
** my_put_unsigned
** File description:
** Function that displays, unsigned number
*/

#include "my.h"

void my_put_unsigned(unsigned int nb)
{
    if (nb > 9) {
        my_put_nbr(nb / 10);
    }
    my_putchar(nb % 10 + 48);
}