/*
** EPITECH PROJECT, 2019
** animator
** File description:
** animator
*/

#ifndef ANIMATOR_H
#define ANIMATOR_H

#define KEYS_ANIMATION_TABLE    \
        KEY(pos)           \
        KEY(scale)              \
        KEY(rotation)           \
        KEY(sprite)

typedef enum key_type
{
    #define KEY(name) \
    key_##name,
    KEYS_ANIMATION_TABLE
    #undef KEY
} key_type;

typedef struct keyframe
{
    float key_x;
    union
    {
        sfVector2f value;
        sfColor color;
    };
} keyframe_t;


typedef struct animation
{
    int entity_id;
    float current_time;
    int loop;
    int end_time;
    float speed;

    #define KEY(name) \
    int name##_current_key;         \
    int name##_next_key;            \
    float name##_perc;              \
    float name##_time;              \
    vector name##_keys;
    KEYS_ANIMATION_TABLE
    #undef KEY
} animation_s;

typedef struct animator
{

} animator_t;

void animation_add_key(animation_s *anim, key_type type,
                    float pos_time, ...);
void animation_reset_timeline(animation_s *anim);
void animation_play(animation_s *anim, float delta);
void animation_init(animation_s *anim, int id, int loop);

void animation_sprite_color(animation_s *anim, float delta);

#endif