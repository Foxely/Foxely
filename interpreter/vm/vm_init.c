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

void reset_stack(vm_t *vm)
{
    vm->stack_top = vm->stack;
    vm->frame_count = 0;
    vm->open_upvalues = NULL;
}

void init_vm(vm_t *vm)
{
    reset_stack(vm);
    vm->objects = NULL;
    vm->bytes_allocated = 0;
    vm->next_gc = 1024 * 100;
    vm->gray_count = 0;
    vm->gray_capacity = 0;
    vm->gray_stack = NULL;
    init_table(&vm->globals);
    init_table(&vm->strings);
    init_table(&vm->imports);

    vm->init_string = NULL;
    vm->init_string = copy_string(vm, "init", 4);

    define_native(vm, &vm->globals, "clock", clock_native);
    load_iolib(vm);
    load_mathlib(vm);
    luaopen_mylib(vm);
}

void free_vm(vm_t *vm)
{
    free_table(vm, &vm->globals);
    free_table(vm, &vm->strings);
    free_table(vm, &vm->imports);
    vm->init_string = NULL;
    free_objects(vm);
}