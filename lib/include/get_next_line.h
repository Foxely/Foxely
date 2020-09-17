/*
** EPITECH PROJECT, 2019
** get_next_line
** File description:
** Get Next Line.h
*/

#ifndef READ_SIZE
#define READ_SIZE (40)
#endif

#ifndef GET_NEXT_LINE_H_
#define GET_NEXT_LINE_H_
    #include <unistd.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <fcntl.h>

    char *get_next_line(int fd);
#endif