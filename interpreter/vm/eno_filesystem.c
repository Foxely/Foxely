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
    {"readfile", readfile_native},
    {"writefile", writefile_native},
    { NULL, NULL }
};

void load_iolib(vm_t *vm)
{
    define_lib(vm, "io", (lib_reg *) reg);
}