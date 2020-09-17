/*
** EPITECH PROJECT, 2019
** fsprintf
** File description:
** File my_sprintf
*/

#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "my.h"

void print(char **out, const char *format, va_list args);

static void example(char *buffer, char *format, va_list args)
{
    print(&buffer, format, args);
}

void fsprintf(int fd, char *format, ...)
{
    va_list args;
    char buffer[255] = "";

    va_start(args, format);
    example(buffer, format, args);
    va_end(args);
    write(fd, buffer, my_strlen(buffer));
}