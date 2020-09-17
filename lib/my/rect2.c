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
#include "rect.h"

v2f v4_min(v4 self)
{
    return v2_new(self.x, self.y);
}

v2f v4_max(v4 self)
{
    return v2_new(self.x + self.w, self.y + self.h);
}

v2f v4_size(v4 self)
{
    return v2_new(self.w, self.h);
}

v4 v4_minowski_diff(v4 self, v4 other)
{
    v2f top_left = v2_sub(v4_min(self), v4_max(other));
    v2f new_size = v2_add(v4_size(self), v4_size(other));
    v2f new_left = v2_add(top_left, v2_div(new_size, 2));
    return v4_create_centered(new_left.x, new_left.y, new_size.x, new_size.y);
}

v2f v4_closest_point_on_bounds(v4 self, v2f point)
{
    float min_dist = ABS(point.x - self.x);
    v2f max = v4_max(self);
    v2f bounds_point = v2_new(self.x, point.y);

    if (ABS(max.x - point.x) < min_dist) {
        min_dist = ABS(max.x - point.x);
        bounds_point = v2_new(max.x, point.y);
    }
    if (ABS(max.y - point.y) < min_dist) {
        min_dist = ABS(max.y - point.y);
        bounds_point = v2_new(point.x, max.y);
    }
    if (ABS(self.y - point.y) < min_dist) {
        min_dist = ABS(self.y - point.y);
        bounds_point = v2_new(point.x, self.y);
    }
    return (bounds_point);
}