/*
** EPITECH PROJECT, 2019
** ecs
** File description:
** Ecs container
*/

#include "csfml.h"
#include "components/transform.h"
#include "components/sprite.h"
#include "components/physics.h"
#include "components/animation.h"
#include "components/collision.h"
#include "components/button.h"
#include "components/draggable.h"
#include "components/text.h"
#include "components/collider.h"
#include "components/audio.h"
#include "components/animator.h"
#include "components/tilemap.h"

#define STR(x) #x
#define RSTR(x) STR(x)

#define LOG_WARNING(msg, ...) \
    my_printf("\33[33mWARNING:\33[0m" msg, ##__VA_ARGS__);
#define LOG_ERROR(msg, ...) my_printf("\33[31mERROR:\33[0m" msg, ##__VA_ARGS__);

#define CORE_COMPONENT(core, id) \
        core_new_component(core, #id);

#define CORE_ENTITY(core, ent_name, count, ...) \
        entity *ent_name = NULL; \
        ent_name = core_new_entity(core, #ent_name, count, __VA_ARGS__);

#define CORE_SET(core, entity, component, ...)  \
    core_set_entity(core, entity, STR(component), &(component)__VA_ARGS__);

#define CORE_SET_PTR(core, entity, component, ...)  \
    core_set_entity(core, entity, STR(component), __VA_ARGS__);

static const float GRAVITY = 9.81f;

typedef struct s_scene scene_t;
typedef struct s_core core_t;

typedef struct v4
{
    float x;
    float y;
    float w;
    float h;
} v4;

typedef struct health_component
{
    int entity_id;
    int component_id;
    int current_health;
    int max_health;
} health_component_t;

typedef struct enemy_component
{
    int entity_id;
    int component_id;
    int current_path;
    int damage;
    float health;
    float speed;
} enemy_cpt;

typedef struct s_component
{
    void **data;
    usize_t size;
} component_t;

typedef struct entity
{
    usize_t id;
    char name[20];
    void *components[100];
    bool enable;
} entity;

bool add_component(core_t *core, u64_t id, const char *name, void *data);
void json_to_entity(core_t *core, json_t *json);

typedef transform_cpt xform_cpt;
typedef animation_component_t anim_t;
typedef physics_cpt physic_t;
typedef enemy_cpt enemy_t;
typedef collision_component_t collision_t;
typedef health_component_t health_t;
typedef drag_component_t drag_t;

void destroy_entity(scene_t *current, int id);
void destroy_sprites(core_t *core);


bool components_set(component_t *comp, u64_t id, void *value);

void destroy_sprites_components(sprite_cpt sprite[], int count);
void destroy_buttons_components(button_cpt buttons[], int count);
void destroy_text_components(text_cpt text[], int count);

void button_system(core_t *core);
void button_system_draw(core_t *core);
void sprite_system(core_t *core);
void transform_system(core_t *core);
void text_system(core_t *core);
void collider_system(core_t *core);
void update_collider_rect(core_t *core, collider_cpt *collider);
void physics_system(core_t *core);

void update_health(core_t *core, health_component_t health[], int count);
void update_animation(core_t *core, animation_component_t anim[], int count);
void update_drag(core_t *core, drag_t drag[], int count);