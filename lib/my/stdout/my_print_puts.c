/*
** EPITECH PROJECT, 2019
** my_print_puts
** File description:
** puts the print
*/

#include <unistd.h>
#include "my.h"

void print_char_void(char **str, void *c)
{
    if (str) {
        **str = (long) c;
        ++(*str);
    } else
        my_putchar((long) c);
}

void print_char(char **str, int c)
{
    if (str) {
        **str = c;
        ++(*str);
        **str = '\0';
    } else
        my_putchar(c);
}

void print_str(char **str, void *s)
{
    if (str) {
        for (size_t i = 0; ((char *)s)[i]; i++)
            print_char(str, ((char *)s)[i]);
    } else
        my_putstr((char *) s);
}

void print_nbr(char **str, void *nbr)
{
    if (str) {
        if ((long) nbr < 0) {
            print_char(str, '-');
            *(int *) nbr *= -1;
        }
        if ((long) nbr > 9)
            print_nbr(str, ((void *) ((long) nbr / 10)));
        print_char(str, (long) nbr % 10 + 48);
    } else
        my_put_nbr((long) nbr);
}

void print_base(char **str, void *nbr, const char *base)
{
    if (str) {
        int base_size = my_strlen(base);

        if ((long) nbr / base_size > 0)
            print_base(str, (void *) ((long) nbr / base_size), base);
        print_char(str, base[(long) nbr % base_size]);
    } else
        my_putbase((long) nbr, base);
}