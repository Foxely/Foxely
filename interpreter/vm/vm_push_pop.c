/*
** EPITECH PROJECT, 2019
** EpiProjectGen
** File description:
** main
*/

#include <stdint.h>
#include "vm.h"

void push(vm_t *vm, value val)
{
    *vm->stack_top = val;
    *vm->stack_top++;
}

value pop(vm_t *vm)
{
    *vm->stack_top--;
    return *vm->stack_top;
}

value peek(vm_t *vm, int distance)
{
    return vm->stack_top[-1 - distance];
}