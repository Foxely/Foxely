/*
** EPITECH PROJECT, 2019
** tilemap
** File description:
** tilemap component
*/

typedef struct tilemap_component
{
    int entity_id;
    int component_id;
    char **map;
    int map_row;
    int map_col;
    v2f cell_size;
} tilemap_cpt;