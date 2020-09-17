/*
** EPITECH PROJECT, 2019
** load_file_in_mem
** File description:
** Load and allocate memory of the file
*/

#include <stdlib.h>
#include "my.h"
#include "get_next_line.h"

char *load_file_in_mem(char const *filepath)
{
    int fd = fs_open_file(filepath, "r");
    char *buffer = NULL;

    for (char *line = get_next_line(fd); line; line = get_next_line(fd)) {
        line = my_strcat_dup(line, "\n");
        buffer = my_strcat_dup(buffer, line);
    }
    close (fd);
    return (buffer);
}