/*
** EPITECH PROJECT, 2019
** physics
** File description:
** Physics component
*/

typedef struct physics_component
{
    int entity_id;
    int component_id;
    sfVector2f velocity;
    int is_static;
    float gravity_scale;
    float bounce_mult;
    float mass;
} physics_cpt;

/**
*   \brief Add the physics component in the entity
*   \param is_static : When is true (1) the entity
                will not be affected by the gravity
*   \param gravity_scale : It is the multiplier of gravity,
                            the higher the value,
                            the greater the gravity of the entity.
*/