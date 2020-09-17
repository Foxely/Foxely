/*
** EPITECH PROJECT, 2019
** my_strtol
** File description:
** Func convert char to int and return the rest
*/

#include "my.h"

int number(char **str)
{
    int nbr = 0;
    int neg = 1;

    while (*str[0] == '+' || *str[0] == '-') {
        if (*str[0] == '-')
            neg *= -1;
        *str += 1;
    }
    while (my_char_isnum(*str[0])) {
        nbr *= 10;
        nbr += *str[0] - '0';
        *str += 1;
    }
    return (nbr * neg);
}

int my_strtol(char *str, char **endptr)
{
    int num = number(&str);
    char *p = str;

    for (int i = 0; *p != '\0'; i++) {
        if (!my_char_isnum(*p))
            break;
        p++;
    }
    *endptr = p;
    return (num);
}

double number_double(char **str)
{
    double rez = 0, fact = 1;
    while (**str == '-') {
        *str += 1;
        fact *= -1;
    };
    for (int point_seen = 0; **str; *str += 1) {
        if (**str == '.') {
            point_seen = 1;
            continue;
        }
        int d = **str - '0';
        if (d >= 0 && d <= 9) {
            if (point_seen)
                fact /= 10.0f;
            rez = rez * 10.0f + (float)d;
        } else {
            break;
        }
    }
    return rez * fact;
}

double my_strtod(char *str, char **endptr)
{
    double num = number_double(&str);
    char *p = str;

    for (int i = 0, point_seen = 0; *p != '\0'; i++) {
        (void) point_seen;
        if (*p == '.') {
            point_seen = 1;
            continue;
        }
        if (!my_char_isnum(*p)) {
            break;
        }
        p++;
    }
    if (endptr)
        *endptr = p;
    return (num);
}