/*
** EPITECH PROJECT, 2019
** my_printf
** File description:
** The Ultime Printf
*/

#include <stdarg.h>
#include "my_printf.h"
#include "my.h"
#include <stdio.h>
#include <unistd.h>

flag_t_t flags[] = {
    {"d", &print_nbr},
    {"s", &print_str},
    {"c", &print_char_void},
    {"o", &print_octal},
    {"x", &print_hex},
    {"X", &print_hexx},
    {"b", &print_binary},
    {"#o", &print_prefixe_octal},
    {"#x", &print_prefixe_hex},
    {"#X", &print_prefixe_hexx},
};

static void find_flag(char *f, char **out, va_list args)
{
    for (size_t i = 0; i < 10; i++) {
        if (my_strcmp(f, "%") == 0) {
            print_char(out, '%');
            break;
        }
        if (my_strcmp(flags[i].flag, f) == 0)
            flags[i].normal(out, va_arg(args, void *));
    }
}

void print(char **out, const char *format, va_list args)
{
    char f[5];

    for (; *format != 0; format++) {
        my_memset(f, 0, 5);
        while (*format != '%' && *format)  {
            print_char(out, *format);
            format++;
        }
        if (*format == '\0')
            return;
        format++;
        my_charcat(f, *format);
        if (*f == '#') {
            format++;
            my_charcat(f, *format);
        }
        find_flag(f, out, args);
    }
    va_end(args);
}

void my_printf(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    print(0, format, args);
}