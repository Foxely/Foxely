/*
** EPITECH PROJECT, 2019
** EpiProjectGen
** File description:
** main
*/

#include <stdint.h>
#include "vm.h"
#include "custom_memory.h"
#include "native.h"

static const lib_reg reg[] = {
    {"sin", sin_native},
    {"cos", cos_native},
    { NULL, NULL }
};

void load_mathlib(vm_t *vm)
{
    define_lib(vm, "math", (lib_reg *) reg);
}