/*
** EPITECH PROJECT, 2019
** fs_open_file
** File description:
** Return value
*/

#include <unistd.h>
#include "my.h"
#include <fcntl.h>

int fs_open_file(char const *filepath, char *perm)
{
    int fd = -1;

    if (!filepath)
        return (-1);
    if (!my_strcmp(perm, "r"))
        fd = open(filepath, O_RDONLY);
    if (!my_strcmp(perm, "rw"))
        fd = open(filepath, O_RDWR | O_CREAT);
    if (!my_strcmp(perm, "w"))
        fd = open(filepath, O_WRONLY | O_CREAT, 0644);
    if (!my_strcmp(perm, "a"))
        fd = open(filepath, O_RDWR | O_CREAT | O_APPEND, 0644);
    if (!my_strcmp(perm, "ta"))
        fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC | O_APPEND, 0644);

    if (fd != -1)
        return (fd);
    else
        return (-1);
}