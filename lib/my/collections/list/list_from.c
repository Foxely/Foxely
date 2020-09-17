/*
** EPITECH PROJECT, 2019
** list_from
** File description:
** transform a elemnt to a list
*/

#include <stdarg.h>
#include "my.h"
#include "private/list_internal.h"

list_t *list_from_arr(usize_t count, void **data)
{
    int err = 0;
    list_t *ls = new_list();

    if (!ls)
        return (NULL);
    while (!err && count--)
        err = list_push_back(ls, *(data++));
    if (err) {
        list_destroy(ls);
        return (NULL);
    }
    return (ls);
}

list_t *list_from_var(usize_t count, va_list ap)
{
    list_t *ls = new_list();
    int err = 0;

    if (!ls)
        return (NULL);
    while (!err && count--)
        err = list_push_back(ls, va_arg(ap, void *));
    if (err) {
        list_destroy(ls);
        return (NULL);
    }
    return (ls);
}

list_t *list_from(usize_t count, ...)
{
    list_t *ls = NULL;
    va_list ap;

    va_start(ap, count);
    ls = list_from_var(count, ap);
    va_end(ap);
    return (ls);
}