/*
** EPITECH PROJECT, 2019
** my_strchr
** File description:
** finding occurrence of a character in a string
*/

char *my_strchr(char *str, int c)
{
    do {
        if (*str == c)
            return (char *)str;
    } while (*str++);
    return (0);
}