/*
** EPITECH PROJECT, 2019
** my_strncmp
** File description:
** Function that if strings are equals or not
*/

int my_strncmp(char const *s1, char const *s2, int n)
{
    for (int i = 0; s1[i] && s2[i] && i < n; i++)
        if (s1[i] != s2[i])
            return ((int) s1[i] - s2[i]);
    return (0);
}