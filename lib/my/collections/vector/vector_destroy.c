/*
** EPITECH PROJECT, 2019
** vector_delete
** File description:
** Delete a node
*/

#include <stdlib.h>
#include "my.h"
#include "private/vector_internal.h"

int vector_destroy(vector *vector)
{
    if (vector == NULL)
        return -1;
    vector->data = NULL;
    return 0;
}