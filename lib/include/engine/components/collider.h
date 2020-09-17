/*
** EPITECH PROJECT, 2019
** position
** File description:
** Position component
*/

#define TAG_UNDEFINED (1 << 0)
#define TAG_ENEMY (1 << 1)
#define TAG_PLAYER (1 << 2)
#define TAG_OBSTACLE (1 << 3)
#define TAG_TRANSITION_UP (1 << 4)
#define TAG_TRANSITION_DOWN (1 << 5)
#define TAG_TRANSITION_LEFT (1 << 6)
#define TAG_TRANSITION_RIGHT (1 << 7)
#define TAG_OLD_MAN (1 << 8)
#define TAG_SWORD (1 << 9)

typedef struct collider_component
{
    int entity_id;
    int component_id;
    sfFloatRect collider;
    int collision_type;
    int collision_against;
    int is_trigger;
    int has_trigger;
    core_t *core;
} collider_cpt;

collider_cpt *create_collision(core_t *core, int entity_id, int tag, int flag);
collider_cpt *create_trigger(core_t *core, int entity_id, int tag, int flag);