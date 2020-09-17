/*
** EPITECH PROJECT, 2020
** quest.h
** File description:
** quest
*/

typedef struct quest_t {
    sfFont *font;
    sfText *obj_text;
    char *text;
    int pnj_name;
} quest_t;

typedef enum obj{
    VIEUX, ENEMY, RING, MC
} obj_t;

void init_quest(quest_t *quest);
void init_text_quest(core_t *game, const char *text, int pnj_name);
void draw_text_quest(core_t *game);
int quest(core_t *game);