/*
** EPITECH PROJECT, 2019
** core
** File description:
** core container
*/

#ifndef _ENGINE_H_
#define _ENGINE_H_

#define GEN_ID (int)(__FILE__ STR(__LINE__))

#define SET_FLAG(n, f) ((n) |= (f))
#define CLR_FLAG(n, f) ((n) &= ~(f))
#define TGL_FLAG(n, f) ((n) ^= (f))
#define CHK_FLAG(n, f) ((n) & (f))

#include "world.h"
#include "input.h"
#include "quest.h"

typedef struct inventory inventory_cpt;
typedef struct item item_t;
typedef struct s_system system_t;

typedef struct s_core
{
    world_t *world;
    hashmap_t *texture_manager;
    hashmap_t *component_manager;
    scene_manager_t scene_manager;
    sfRenderWindow *window;
    sfEvent e;
    sfView *camera;
    int exit;
    float dt;
    bool pause;

    list_t *on_load_system;
    list_t *post_load_system;
    list_t *pre_update_system;
    list_t *on_update_system;
    list_t *on_validate_system;
    list_t *post_update_system;
    list_t *pre_store_system;
    list_t *on_store_system;
    list_t *on_pollevent_system;
    list_t *on_pause_system;


    list_t *on_add_system;
    list_t *on_remove_system;
    list_t *on_rset_system;

    input_t input;

    quest_t quest;
    bool dialogue_start;
    bool dialogue_refresh;
    int status;
    inventory_cpt *inv;
    item_t *ring;
    bool gameover;
} core_t;

typedef enum s_core_event
{
    on_load,
    post_load,
    pre_update,
    on_update,
    on_validate,
    post_update,
    pre_store,
    on_store,
    on_poll_event,
    on_pause,


    on_add,
    on_remove,
    on_set
} core_event_t;


core_t *new_core(void);
void core_new_component(core_t *core, char *id);
entity *core_new_entity(core_t *core, char *name, usize_t comp_count, ...);
void core_set_entity(core_t *core, entity *ent, const char *id, void *data);
void *core_get(core_t *core, const char *id, int entity_id);
entity *core_get_entity(core_t *core, const int entity_id);
entity *core_get_entity_by_name(core_t *core, const char *name);
void entity_set_name(entity *ent, char *name);
void destroy_core(core_t *core);

void *components_get(component_t *self, int id);
void enemy_system(core_t *core);

int app_update(core_t *core);
void close_window(sfRenderWindow *window);
void init_window(core_t *core);

void destroy_entities(core_t *core);
int find_entity_id_by_name(scene_t *current, char *name);
entity *find_entity_by_name(scene_t *current, char *name);

void init_sprites(core_t *core);
void update_systems(core_t *core);
void render_components(core_t *core);

void trigger_enter(collider_cpt *other1, collider_cpt *other2);
void trigger_stay(collider_cpt *other1, collider_cpt *other2);
void trigger_exit(collider_cpt *other1, collider_cpt *other2);

sfVector2f vector2_normalize(float x, float y);
float vector2_distance(float x1, float x2, float y1, float y2);

void sprite_set_color(sprite_cpt *sp, sfColor color);
sprite_cpt *create_sprite(core_t *core, char *texture_id, int entity_id);
button_cpt *create_button(core_t *core, char *texture_id, int entity_id,
                        void (*handler)(core_t *, button_cpt *));

void load_scene(core_t *core, char *json_path);
void tile_map(core_t *core, json_t *tilemap);
enemy_cpt *create_enemy(int entity_id);

#endif