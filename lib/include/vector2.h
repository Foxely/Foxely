/*
** EPITECH PROJECT, 2019
** my_char_to_str
** File description:
** Function that verify that the char is only number
*/

#include <stdlib.h>
#include <math.h>
#include "my.h"
#include "core.h"

v2f v2_new(float x, float y);
float v2_mag(v2f self);
v2f v2_add(v2f v1, v2f v2);
v2f v2_sub(v2f v1, v2f v2);
v2f v2_mul(v2f self, float val);
v2f v2_div(v2f self, float val);
v2f v2_unit(v2f self);

sfVector2f move_towards(sfVector2f from, sfVector2f to, float delta);