/*
** EPITECH PROJECT, 2019
** input
** File description:
** input container
*/

#ifndef _INPUT_H_
#define _INPUT_H_

#include "hashmap.h"

typedef enum
{
    UNDEFINED,
    KEY_DOWN,
    KEY_PRESS,
    KEY_RELEASE,

    MOUSE_CLICK,
    MOUSE_RELEASE,
} input_state;

typedef struct s_key
{
    char *type;
    int keycode;
    int mouse_button;
} key_input_t;


typedef struct s_input
{
    hashmap_t *keys;
    input_state state;
    int current_key;
    bool pressed;
} input_t;

void input_init(core_t *core);
void input_destroy(core_t *core);
void input_update(core_t *core);
void input_poll(core_t *core);

bool input_get_button(core_t *core, char *name);
bool input_get_button_down(core_t *core, char *name);
bool input_get_button_up(core_t *core, char *name);
bool input_get_mouse_button_down(core_t *core, char *name);

#endif