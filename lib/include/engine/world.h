/*
** EPITECH PROJECT, 2019
** core
** File description:
** core container
*/

#ifndef _WORLD_H_
#define _WORLD_H_

#include "core.h"

typedef void (*system_func_t)(core_t *);

typedef struct s_world
{
    system_func_t func;
} world_t;
#endif