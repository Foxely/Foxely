/*
** EPITECH PROJECT, 2019
** sprite
** File description:
** Sprite component
*/

typedef struct sprite_component
{
    int entity_id;
    int component_id;
    char *texture_id;
    sfSprite *sprite;
    sfColor color;
    int flip_x;
    int flip_y;
    int is_flip_x;
    int is_flip_y;
} sprite_cpt;