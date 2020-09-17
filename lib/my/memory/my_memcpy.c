/*
** EPITECH PROJECT, 2019
** my_memcpy
** File description:
** mymemcpy
*/

#include <stdio.h>

void *my_memcpy(void *dest, const void *src, size_t len)
{
    char *d = dest;
    const char *s = src;
    while (len--)
        *d++ = *s++;
    return (dest);
}