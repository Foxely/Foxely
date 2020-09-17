/*
** EPITECH PROJECT, 2019
** mem_alloc_2d_array
** File description:
** allocate a 2d Array and return his address
*/

/**
* \file mem_alloc_2d_array.c
* \author BESSEAU Samuel
* \version 1
* \date November 24th
*/

#include <stdlib.h>

char **mem_alloc_2d_array(int nb_rows, int nb_cols)
{
    char **array = (char **)malloc(nb_rows * sizeof(char *));
    for (int i = 0; i < nb_rows; i++)
        array[i] = (char *)malloc(nb_cols * sizeof(char));
    return (array);
}

int **mem_alloc_2d_array_int(int nb_rows, int nb_cols)
{
    int **array = (int **)malloc(nb_rows * sizeof(int *));
    for (int i = 0; i < nb_rows; i++)
        array[i] = (int *)malloc(nb_cols * sizeof(int));
    return (array);
}