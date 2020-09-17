/*
** EPITECH PROJECT, 2019
** animation
** File description:
** Animation component
*/

typedef struct animation_component
{
    int entity_id;
    int component_id;
    sfIntRect rect;
    int current_frame;
    float interval_mult;
    float time;
} animation_component_t;