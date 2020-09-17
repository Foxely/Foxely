/*
** EPITECH PROJECT, 2019
** my_show_word_array
** File description:
** Func that displaythe content of an array of words
*/

#include "my.h"

int my_show_word_array(char * const *tab)
{
    for (int i = 0; tab[i]; i++) {
        my_putstr(tab[i]);
        my_putchar('\n');
    }
    return (0);
}