/*
** EPITECH PROJECT, 2019
** my_print_bases
** File description:
** print base
*/

#include "my.h"

void print_hexx(char **str, void *nbr)
{
    print_base(str, nbr, "0123456789ABCDEF");
}

void print_hex(char **str, void *nbr)
{
    print_base(str, nbr, "0123456789abcdef");
}

void print_octal(char **str, void *nbr)
{
    print_base(str, nbr, "01234567");
}

void print_binary(char **str, void *nbr)
{
    print_base(str, nbr, "01");
}