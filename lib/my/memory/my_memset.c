/*
** EPITECH PROJECT, 2019
** my_memset
** File description:
** like_memset
*/

void *my_memset(void *ptr, int x, int n)
{
    unsigned char *temp = ptr;
    for (int i = 0; i < n; i++)
        *temp++ = x;
    return ptr;
}