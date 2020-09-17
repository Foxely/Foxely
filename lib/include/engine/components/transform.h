/*
** EPITECH PROJECT, 2019
** position
** File description:
** Position component
*/

typedef struct s_core core_t;

typedef struct transform_component
{
    int entity_id;
    int component_id;
    double pos_x;
    double pos_y;
    double rot_x;
    double rot_y;
    double scale_x;
    double scale_y;
    int parent;
    sfTransform local;
    sfTransform world;
} transform_cpt;

void set_position(core_t *core, int id, double x, double y);
sfVector2f get_position(core_t *core, int id);
void set_scale(core_t *core, int id, double x, double y);
sfVector2f get_scale(core_t *core, int id);
void set_parent(core_t *core, int id, int parent);
int get_parent(core_t *core, int id);