/*
** EPITECH PROJECT, 2019
** core
** File description:
** core container
*/

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "core.h"

typedef struct s_system_data system_data_t;
typedef void (*system_func_t)(core_t *);

typedef struct s_system
{
    system_func_t func;
    system_data_t *data;
} system_t;

typedef struct s_system_data
{
    list_t *components;
    list_t *entities;
} system_data_t;

bool system_add(core_t *core, core_event_t event, system_func_t func);
void notify_systems(core_t *core, list_t *system);
bool system_match(core_t *core, list_t *systems, entity *entity);

#endif