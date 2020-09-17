/*
** EPITECH PROJECT, 2019
** button_callback
** File description:
** button callback
*/

#ifndef _CALLBACK_H_
#define _CALLBACK_H_

void quit(core_t *, button_cpt *btn);
void play(core_t *, button_cpt *btn);
void resume(core_t *core, button_cpt *btn);
void go_to_menu(core_t *core, button_cpt *btn);

typedef struct callback_t
{
    char name[20];
    void (*cb)(core_t *, button_cpt *);
} cb_t;

static const cb_t btn_callbacks[10] = {
    {"play", &play},
    {"quit", &quit},
    {"resume", &resume},
    {"go_to_menu", &go_to_menu},
};

#endif