/*
** EPITECH PROJECT, 2019
** my_getnbr
** File description:
** Function that returns a number, sent to the function as a string.
*/

#include "my.h"

static int how_many_digit(char const *str)
{
    int count = 0;
    for (int i = 0; i < my_strlen(str); i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            count++;
        }
    }
    return (count);
}

int condition(char const *str, int i, long *p_verif, unsigned long *p_verif2)
{
    int number_of_digit = how_many_digit(str);

    if (*p_verif > 2147483647) {
        if ((str[i] < 'a' && str[i] > 'z') || number_of_digit >= 10)
            return (0);
    }
    if (*p_verif2 < (unsigned long) -2147483648) {
        if ((str[i] < 'a' && str[i] > 'z') || number_of_digit >= 10)
            return (0);
    }
    return (1);
}

static int condition_character(char const *str, int i, int *p_neg, int found_n)
{
    if (str[i] == '-')
        *p_neg *= -1;

    if (my_char_isalpha(str[i]) || str[i] == '+' || (str[i] == '.' && found_n))
        return (0);
    return (1);
}

static int loop_get_nbr(char const *str, int *nb, int *neg)
{
    long verif = 0;
    unsigned long verif2 = 0;
    int found_num = 0;

    for (int i = 0; str[i] != '\0'; i++) {
        verif = (long) *nb;
        verif2 = (unsigned long) *nb;

        if (my_char_isnum(str[i])) {
            *nb += str[i] - '0';
            *nb *= 10;
            found_num = 1;
            if (condition(str, i, &verif, &verif2) == 0)
                return (2);
        } else
            if (condition_character(str, i, neg, found_num) == 0)
                return (3);
    }
    return (0);
}

int my_getnbr(char const *str)
{
    int neg = 1;
    int nb = 0;

    int cond = loop_get_nbr(str, &nb, &neg);

    if (cond == 2) {
        return (0);
    }
    nb /= 10;

    return (neg * nb);
}