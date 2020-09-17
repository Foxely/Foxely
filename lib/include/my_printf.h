/*
** EPITECH PROJECT, 2019
** my
** File description:
** Header contains all functions prototyped
*/

#ifndef _MY_PRINTF_
#define _MY_PRINTF_

#include <stdio.h>

/********************************STRUCT***********************************/

typedef struct {
    char *flag;
    void (*normal)(char **str, void *c);
} flag_t_t;

/****************************************************************************/
/****************************PUTS FUNCTION***********************************/

void my_putstr(char const *str);
void my_put_float(double nb);
void my_putchar(char c);
void my_put_nbr(int nb);
void my_putbase(int nb, const char *base);
void my_put_unsigned(unsigned int nb);
void my_printf(const char *, ...);
void my_sprintf(char *buffer, const char *, ...);
void fsprintf(int fd, char *format, ...);

void print_char(char **str, int c);
void print_char_void(char **str, void *c);
void print_str(char **str, void *s);
void print_nbr(char **str, void *nbr);
void print_base(char **str, void *nbr, const char *base);
void print_hex(char **str, void *nbr);
void print_hexx(char **str, void *nbr);
void print_octal(char **str, void *nbr);
void print_binary(char **str, void *nbr);
void print_prefixe_hexx(char **str, void *nbr);
void print_prefixe_hex(char **str, void *nbr);
void print_prefixe_octal(char **str, void *nbr);

/****************************************************************************/

#endif