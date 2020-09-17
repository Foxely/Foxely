/*
** EPITECH PROJECT, 2019
** health
** File description:
** Health component
*/

typedef struct position_component
{
    int entity_id;
    int component_id;
    int x;
    int y;
} position_component_t;

/**
*   \brief Add the position component in the entity
*   \param x : The X position of the entity in the world position
*   \param y : The Y position of the entity in the world position
*/
void position_entity(float x, float y);