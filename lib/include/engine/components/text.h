/*
** EPITECH PROJECT, 2019
** sprite
** File description:
** Sprite component
*/

typedef struct text_component
{
    int entity_id;
    int component_id;
    sfText *text;
    sfFont *font;
    char string[1024];
} text_cpt;