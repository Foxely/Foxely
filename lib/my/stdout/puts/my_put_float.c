/*
** EPITECH PROJECT, 2019
** my_put_float
** File description:
** Function that displays, all the combinations of two two-digit numbers.
*/

#include "my.h"

void my_put_float(double nb)
{
    int	i;
    int	entire_part;
    double	decimal_part;

    i = 100;
    entire_part = (int)(nb / 1);
    decimal_part = (nb - entire_part) * i;
    if (decimal_part < 0)
        decimal_part *= -1;
    my_put_nbr(entire_part);
    my_putchar('.');
    if (decimal_part) {
        while (decimal_part / (i = i / 10) == 0 && i > 0)
            my_putchar('0');
    }
    my_put_nbr((int)decimal_part);
}