/*
** EPITECH PROJECT, 2019
** button
** File description:
** Button component
*/

#include "my_event.h"

typedef struct button_component
{
    int entity_id;
    int component_id;
    sfColor normal_color;
    sfColor hover_color;
    sfColor pressed_color;
    sfFloatRect rect;
    sfSprite *sprite;
    listener_l listeners;
} button_cpt;

/**
*   \brief Add a listener into the click event
*   \param button : The Reference of the button
*   \param name : The name of the listener
*   \param handler : This is the callback of the listener
*/
void button_on(button_cpt *btn, char *name,
                void (*handler)(core_t *, void *), void *data);

/**
*   \brief Add a listener into the click event
*   \param color : The Reference of the button color like pressed color
*   \param r : The red color value
*   \param g : The green color value
*   \param b : The bleu color value
*/
void set_button_color(sfColor *color, sfUint8 r, sfUint8 g, sfUint8 b);

void button_raise(core_t *core, button_cpt *btn);