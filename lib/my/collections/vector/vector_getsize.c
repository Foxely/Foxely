/*
** EPITECH PROJECT, 2019
** my_list_size
** File description:
** Func that returns the number of elemnts onthe list
*/

#include <stdlib.h>
#include "private/vector_internal.h"
#include "vector.h"

int vector_getsize(vector *begin)
{
    return (begin->size);
}