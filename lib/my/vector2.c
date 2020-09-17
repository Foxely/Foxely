/*
** EPITECH PROJECT, 2019
** my_char_to_str
** File description:
** Function that verify that the char is only number
*/

#include <stdlib.h>
#include "my.h"
#include "core.h"
#include "vector2.h"

v2f v2_mul(v2f self, float val)
{
    return (v2f){self.x * val, self.y * val};
}

v2f v2_div(v2f self, float val)
{
    return (v2f){self.x / val, self.y / val};
}

v2f v2_unit(v2f self)
{
    float mag = v2_mag(self);
    return (v2f){self.x / mag, self.y / mag};
}