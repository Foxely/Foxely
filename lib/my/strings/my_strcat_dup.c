/*
** EPITECH PROJECT, 2019
** my_strcat_dup
** File description:
** combine my_strcat and my_strdup
*/

#include "my.h"

char *my_strcat_dup(char *str1, char *str2)
{
    if (!str1)
        return (str2);
    if (!str2)
        return (str1);
    char *new = malloc(sizeof(char) *
                                    (my_strlen(str1) + my_strlen(str2) + 1));
    if (new) {
        my_strncpy(new, str1, my_strlen(str1));
        my_strncat(new, str2, my_strlen(str2));
        free(str1);
        free(str2);
        return (new);
    }
    return (NULL);
}
