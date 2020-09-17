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

v2f v2_new(float x, float y)
{
    v2f self = {x, y};
    return (self);
}

float v2_mag(v2f self)
{
    return sqrt(pow(self.x, 2) + pow(self.y, 2));
}

v2f v2_add(v2f v1, v2f v2)
{
    return (v2f){v1.x + v2.x, v1.y + v2.y};
}

v2f v2_sub(v2f v1, v2f v2)
{
    return (v2f){v1.x - v2.x, v1.y - v2.y};
}