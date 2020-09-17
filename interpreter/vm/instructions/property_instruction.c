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
property_set(vm_t *vm, call_frame *frame, uint8_t instruction)
{
    if (instruction == OP_SET_PROPERTY) {
        if (!IS_INSTANCE(peek(vm, 1))) {
            runtime_error(vm, "Only instances have fields.");
            return INTERPRET_RUNTIME_ERROR;
        }
        obj_instance *instance = AS_INSTANCE(peek(vm, 1));
        table_set(vm, &instance->fields,
                    READ_STRING(frame), peek(vm, 0));

        value value = pop(vm);
        pop(vm);
        push(vm, value);
    }
    return INTERPRET_OK;
}

interpret_result
property_instruction(vm_t *vm, call_frame *frame, uint8_t instruction)
{
    if (instruction == OP_GET_PROPERTY) {
        if (!IS_INSTANCE(peek(vm, 0))) {
            runtime_error(vm, "Only instances have properties.");
            return INTERPRET_RUNTIME_ERROR;
        }
        obj_instance *instance = AS_INSTANCE(peek(vm, 0));
        obj_string *name = READ_STRING(frame);
        value value;
        if (table_get(&instance->fields, name, &value)) {
            pop(vm);
            push(vm, value);
            return (INTERPRET_OK);
        }
        if (!bind_method(vm, instance->klass, name))
            return INTERPRET_RUNTIME_ERROR;
    } else
        return (property_set(vm, frame, instruction));
    return INTERPRET_OK;
}