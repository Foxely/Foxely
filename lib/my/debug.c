/*
** EPITECH PROJECT, 2019
** debug
** File description:
** debug_assert
*/

#include <unistd.h>
#include "my.h"

void debug(char *format)
{
    write(1, format, my_strlen(format));
}

void assert_failure(char *condition, int line, char *file, int crash)
{
    fsprintf(2, "ASSERTION FAILURE: %s at %s:%i\n", condition, file, line);
    if (crash) {
        *(volatile int *)0 = 0;
    }
}