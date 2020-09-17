/*
** EPITECH PROJECT, 2019
** debug
** File description:
** Debug Function
*/

#ifndef _DEBUG_
#define _DEBUG_
#include <stdio.h>
void debug(char *format);
void assert_failure(char *condition, int line, char *file, int crash);
#define R_DEV_ASSERT(cond, fmt, ret) \
    if (!(cond)) debug(fmt); \
    if (!(cond)) ret;


#define A_SSERT(c, crash) \
        if (!(c)) assert_failure(#c, __LINE__, __FILE__, crash);
#define ASSERT(c) A_SSERT(c, 1)
#define SOFT_ASSERT(c) A_SSERT(c, 0)
#endif