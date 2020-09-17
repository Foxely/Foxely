/*
** EPITECH PROJECT, 2019
** file_manager
** File description:
** Funcs that work with file
*/

#include <fcntl.h>
#include <unistd.h>
#include "my.h"
#include <errno.h>

char *get_file_content(char *file)
{
    char *buffer = my_strdup("");
    char c;
    int fd;
    fd = open(file, O_RDONLY);

    if (fd != -1) {
        while (read(fd, &c, 1) > 0)
            my_charcat(buffer, c);
        close(fd);
    } else {
        my_putstr("Impossible d'ouvrir le fichier \n");
    }
    return (buffer);
}

int read_file(char *file)
{
    char c;
    int fd;

    fd = open(file, O_RDONLY);

    if (fd != -1) {
        while (read(fd, &c, 1) > 0) {
            my_putchar(c);
        }
        close(fd);
    } else {
        my_putstr("Impossible d'ouvrir le fichier \n");
        return (errno);
    }
    return (0);
}

void write_file(char *file, char *data)
{
    int size = my_strlen(data);
    int fd;
    fd = open(file, O_CREAT | O_RDWR, 0644);
    write(fd, data, size);
}
