/*
** EPITECH PROJECT, 2019
** EpiProjectGen
** File description:
** main
*/

#include <stdio.h>
#include <time.h>
#include "vm.h"
#include "chunk_debug.h"
#include "my_printf.h"
#include "object.h"
#include "my.h"

obj_upvalue *capture_upvalue(vm_t *vm, value *local)
{
    obj_upvalue *prev_upvalue = NULL;
    obj_upvalue *upvalue = vm->open_upvalues;

    while (upvalue != NULL && upvalue->location > local) {
        prev_upvalue = upvalue;
        upvalue = upvalue->next;
    }

    if (upvalue != NULL && upvalue->location == local)
        return upvalue;
    obj_upvalue *created_upvalue = new_upvalue(vm, local);
    created_upvalue->next = upvalue;
    if (prev_upvalue == NULL) {
        vm->open_upvalues = created_upvalue;
    } else {
        prev_upvalue->next = created_upvalue;
    }
    return created_upvalue;
}

void close_upvalues(vm_t *vm, value *last)
{
    while (vm->open_upvalues != NULL &&
            vm->open_upvalues->location >= last) {
        obj_upvalue *upvalue = vm->open_upvalues;
        upvalue->closed = *upvalue->location;
        upvalue->location = &upvalue->closed;
        vm->open_upvalues = upvalue->next;
    }
}