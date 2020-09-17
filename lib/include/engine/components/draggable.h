/*
** EPITECH PROJECT, 2019
** position
** File description:
** Position component
*/

typedef struct drag_component
{
    int entity_id;
    int component_id;
    int selected;
    sfFloatRect rect;
} drag_component_t;