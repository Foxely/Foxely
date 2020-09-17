/*
** EPITECH PROJECT, 2019
** get_next_line
** File description:
** Get line in a fd
*/

#include <unistd.h>
#include <stdlib.h>
#include "get_next_line.h"

void malloc_buffer(char **buffer, const int fd)
{
    if (*buffer == NULL || *buffer[0] == '\0') {
        int byte = 0;
        *buffer = malloc(sizeof(char) * (READ_SIZE + 1));
        if (*buffer == NULL)
            return;
        byte = read(fd, *buffer, READ_SIZE);
        if (byte == -1)
            return;
        (*buffer)[byte] = '\0';
    }
}

char *malloc_result(char **result)
{
    *result = malloc(sizeof(char) * (READ_SIZE + 1));
    return (*result);
}

void copy(char *result, char *str1, char *str2, int *j)
{
    for (int i = 0; str1[i]; i++, *j += 1)
        result[*j] = str1[i];
    for (int i = 0; str2[i]; i++, *j += 1)
        result[*j] = str2[i];
}

char *str_cat_dup(char *str1, char *str2)
{
    char *result = NULL;
    int len_str1 = 0;
    int len_str2 = 0;
    int j = 0;

    if (str2 == NULL)
        return (str1);
    if (str1 == NULL)
        return (str2);
    while (str1[len_str1++]);
    while (str2[len_str2++]);
    int len3 = len_str1 + len_str2 + 1;
    result = malloc(sizeof(char) * len3);
    if (result == NULL)
        return (NULL);
    copy(result, str1, str2, &j);
    result[j] = '\0';
    free(str1);
    free(str2);
    return (result);
}

char *get_next_line(int fd)
{
    static char *buffer;
    char *ret = NULL;
    int i = 0;

    malloc_buffer(&buffer, fd);
    if (buffer == NULL || buffer[0] == '\0')
        return (NULL);
    if (malloc_result(&ret) == NULL)
        return (NULL);
    while (buffer[0] != '\0' && (i == 0 || *(buffer - 1) != '\n')) {
        ret[i] = buffer[0];
        buffer += 1;
        i++;
    }
    ret[i] = '\0';
    if (i != 0 && ret[i - 1] == '\n') {
        ret[i - 1] = '\0';
    } else
        ret = str_cat_dup(ret, get_next_line(fd));
    return (ret);
}