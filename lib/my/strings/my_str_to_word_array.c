/*
** EPITECH PROJECT, 2019
** my_str_to_word_array
** File description:
** Func that splits a string into words
*/

#include <stdlib.h>
#include "my.h"
#include "debug.h"
#include "garbage.h"

static int get_col_length(char const *str)
{
    char *dup = my_strdup(str);
    char *ptr = my_strtok(dup, " ");
    int col = my_strlen(ptr);
    for (int i = 0; ptr; i++) {
        int len = my_strlen(ptr);
        if (len > col)
            col = len;
        ptr = my_strtok(NULL, " \t");
    }
    gc_free(get_garbage(), dup);
    return (col);
}

static int get_row_length(char const *str)
{
    char *dup = my_strdup(str);
    char *ptr = my_strtok(dup, " ");
    int row = 0;
    for (int i = 0; ptr; i++) {
        ptr = my_strtok(NULL, " \t");
        row++;
    }
    gc_free(get_garbage(), dup);
    return (row);
}

char **my_str_to_word_array(char *str, char *delim)
{
    R_DEV_ASSERT(str && str[0] != '\0', "", return (NULL));
    int row = get_row_length(str);
    int col = get_col_length(str);
    char **array = mem_alloc_2d_array(row + 1, col);
    char *word = NULL;

    R_DEV_ASSERT(array, "", return (NULL));
    my_memset_array(array, 0, row + 1, col);
    word = my_strtok(str, delim);
    for (int i = 0; word; i++) {
        my_strcpy(array[i], word);
        gc_free(get_garbage(), word);
        word = my_strtok(NULL, delim);
    }
    array[row] = NULL;
    return (array);
}