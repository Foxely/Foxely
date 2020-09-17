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

v4 v4_new(float x, float y, float w, float h)
{
    v4 self = {x, y, w, h};
    return (self);
}

v4 v4_create_centered(float cx, float cy, float w, float h)
{
    v4 self = {0, 0, w, h};
    self.x = cx - self.w / 2;
    self.y = cy - self.h / 2;
    return self;
}

v2f v4_center(v4 self)
{
    return v2_new(self.x + self.w / 2, self.y + self.h / 2);
}

v4 v4_set_center(v4 self, float x, float y)
{
    return (v4_new(x - self.w / 2, y - self.h / 2, self.w, self.h));
}

v4 v4_from_csfml_rect(sfFloatRect rect)
{
    return (v4_create_centered(rect.left, rect.top, rect.width, rect.height));
}