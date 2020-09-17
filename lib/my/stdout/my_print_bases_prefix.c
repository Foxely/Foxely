/*
** EPITECH PROJECT, 2019
** my_print_bases
** File description:
** print base
*/

#include "my.h"

void print_prefixe_hexx(char **str, void *nbr)
{
    print_str(str, "0X");
    print_hexx(str, nbr);
}

void print_prefixe_hex(char **str, void *nbr)
{
    print_str(str, "0x");
    print_hex(str, nbr);
}

void print_prefixe_octal(char **str, void *nbr)
{
    print_str(str, "0");
    print_octal(str, nbr);
}