/*
** EPITECH PROJECT, 2019
** position
** File description:
** Position component
*/

typedef struct audio_comp
{
    int entity_id;
    int component_id;
    sfSoundBuffer *buffer;
    sfSound *audio;
    int loop;
    float pitch;
    float volume;
    void (*del)(void *);
} audio_cpt;

audio_cpt *create_audio(core_t *core, int id, char *file, bool loop);