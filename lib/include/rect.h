/*
** EPITECH PROJECT, 2019
** my_char_to_str
** File description:
** v4 v4_at verify that the char is only number
*/

#include <stdlib.h>
#include <math.h>
#include "my.h"
#include "core.h"
#include "vector2.h"

#define ABS(N) ((N < 0) ?(-N) : (N))

v4 v4_new(float x, float y, float w, float h);
v4 v4_create_centered(float cx, float cy, float w, float h);
v2f v4_center(v4 self);
v4 v4_set_center(v4 self, float x, float y);
v2f v4_min(v4 self);
v2f v4_max(v4 self);
v2f v4_size(v4 self);
v4 v4_minowski_diff(v4 self, v4 other);
v2f v4_closest_point_on_bounds(v4 self, v2f point);
v4 v4_from_csfml_rect(sfFloatRect rect);