/*
** EPITECH PROJECT, 2019
** mylist
** File description:
** Header Struct Container
*/

#ifndef __NATIVE_H__
#define __NATIVE_H__

#include "value.h"

void load_mathlib(vm_t *vm);
void load_iolib(vm_t *vm);
void loadlib(vm_t *vm);
value clock_native(vm_t *vm, int arg_count, value *args);
value sin_native(vm_t *vm, int arg_count, value *args);
value cos_native(vm_t *vm, int arg_count, value *args);
value readfile_native(vm_t *vm, int ac, value *args);
value writefile_native(vm_t *vm, int ac, value *args);

#endif