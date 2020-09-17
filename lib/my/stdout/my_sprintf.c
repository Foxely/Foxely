/*
** EPITECH PROJECT, 2019
** my_printf
** File description:
** The Ultime Printf
*/

#include <stdarg.h>
#include "my.h"
#include <stdio.h>
#include <stdlib.h>

void print(char **out, const char *format, va_list args);

void my_sprintf(char *buffer, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    print(&buffer, format, args);
}