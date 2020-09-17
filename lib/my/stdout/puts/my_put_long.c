/*
** EPITECH PROJECT, 2019
** my_put_long
** File description:
** Put a long number
*/

#include "my.h"

static int display_min_int(void)
{
    my_putchar('-');
    my_putchar('2');
    my_putchar('1');
    my_putchar('4');
    my_putchar('7');
    my_putchar('4');
    my_putchar('8');
    my_putchar('3');
    my_putchar('6');
    my_putchar('4');
    my_putchar('8');
    return (0);
}

void my_put_long(long nb)
{
    if (nb == -2147483648) {
        display_min_int();
    }
    if (nb < 0) {
        my_putchar('-');
        nb *= -1;
    }
    if (nb > 9) {
        my_put_long(nb / 10);
    }
    my_putchar(nb % 10 + 48);
}

void my_put_long_int(long int nb)
{
    if (nb == -2147483648) {
        display_min_int();
    }
    if (nb < 0) {
        my_putchar('-');
        nb *= -1;
    }
    if (nb > 9) {
        my_put_long(nb / 10);
    }
    my_putchar(nb % 10 + 48);
}