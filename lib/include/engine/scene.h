/*
** EPITECH PROJECT, 2019
** scene
** File description:
** Header scene Container
*/

#ifndef _SCENE_
#define _SCENE_

#include "my.h"
#include "ecs.h"
#include "hashmap.h"

typedef struct scene_manager scene_manager_t;

typedef struct s_scene {
    char *name;
    hashmap_t *comps;
    list_t *entities;
    core_t *core;

    void (*on_enable)(struct s_scene *);
    void (*update)(struct s_scene *state, float dt);
    void (*destroy)(struct s_scene *state);
    struct s_scene *next;
} scene_t;

struct scene_manager {
    vector stack;
    int current;
    sfClock *clock;
    sfTime time;
};

int scene_manager_contain(scene_manager_t *scene_manager, scene_t *scene);
scene_manager_t *scene_manager_init(void);
void scene_manager_update(scene_manager_t *scene_manager, const float *dt);
scene_t *scene_manager_top(scene_manager_t *scene_manager);
void scene_manager_loadscene_by_id(core_t *core, size_t id);
void scene_manager_free(scene_manager_t *scene_manager);
void scene_manager_push(scene_manager_t *scene_manager, scene_t *scene);

void save_scene_to_json(scene_t *scene, char *file_name);

#define STATE_INIT(core, v_name, ...)   \
scene_t v_name = {.name = #v_name, .next = NULL, .core = core,    \
                .entities = new_list(), .comps = hashmap_new(), __VA_ARGS__};


#endif