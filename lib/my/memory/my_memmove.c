/*
** EPITECH PROJECT, 2019
** my_memmove
** File description:
** like my_memcpy
*/

#include <stdio.h>

void *my_memmove(void *dest, const void *src, size_t len)
{
    char *d = dest;
    char *s = (char *) src;
    if (d < s)
        while (len--)
            *d++ = *s++;
    else
    {
        char *lasts = s + (len-1);
        char *lastd = d + (len-1);
        while (len--)
        *lastd-- = *lasts--;
    }
    return dest;
}
