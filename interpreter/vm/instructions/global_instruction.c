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

interpret_result
global_set(vm_t *vm, call_frame *frame, uint8_t instruction)
{
    switch (instruction) {
        case OP_SET_GLOBAL: {
            obj_string *name = READ_STRING(frame);
            if (table_set(vm, &vm->globals, name, peek(vm, 0))) {
                table_delete(&vm->globals, name);
                runtime_error(vm, "Undefined variable '%s'.", name->chars);
                return INTERPRET_RUNTIME_ERROR;
            }
            break;
        }
    }
    return INTERPRET_OK;
}

interpret_result
global_instruction(vm_t *vm, call_frame *frame, uint8_t instruction)
{
    obj_string *name = READ_STRING(frame);
    switch (instruction) {
        case OP_GET_GLOBAL: {
            value value;
            if (!table_get(&vm->globals, name, &value)) {
                runtime_error(vm, "Undefined variable '%s'.", name->chars);
                return INTERPRET_RUNTIME_ERROR;
            }
            push(vm, value);
            break;
        }
        case OP_DEFINE_GLOBAL:
            table_set(vm, &vm->globals, name, peek(vm, 0));
            pop(vm);
            break;
        default:
            return (global_set(vm, frame, instruction));
            break;
    }
    return INTERPRET_OK;
}