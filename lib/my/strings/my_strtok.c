/*
** EPITECH PROJECT, 2019
** my_strtok
** File description:
** Parse
*/

#include <stdlib.h>
#include "my.h"

char *str_tok_func(char *s, const char *delim, char **save_ptr);

size_t my_strspn(const char *cs, const char *ct)
{
    size_t n;
    const char *p;
    for (n = 0; *cs; cs++, n++) {
        for (p = ct; *p && *p != *cs; p++);
        if (!*p)
            break;
    }
    return (n);
}

size_t my_strcspn(const char *s1, const char *s2)
{
    const char *s = s1;
    const char *c;

    for (; *s1; s1++) {
        for (c = s2; *c; c++) {
            if (*s1 == *c)
                break;
        }
        if (*c)
            break;
    }
    return s1 - s;
}

char *my_strtok(char *str, char *delim)
{
    static char *old;
    return (str_tok_func(str, delim, &old));
}

char *str_tok_func(char *s, const char *delim, char **save_ptr)
{
    char *end;
    if (s == NULL)
        s = *save_ptr;
    if (*s == '\0') {
        *save_ptr = s;
        return (NULL);
    }
    s += my_strspn(s, delim);
    if (*s == '\0') {
        *save_ptr = s;
        return (NULL);
    }
    end = s + my_strcspn(s, delim);
    if (*end == '\0') {
        *save_ptr = end;
        return (s);
    }
    *end = '\0';
    *save_ptr = end + 1;
    return (s);
}