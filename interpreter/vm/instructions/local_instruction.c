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

void local_instruction(vm_t *vm, call_frame *frame, uint8_t instruction)
{
    switch (instruction) {
        case OP_GET_LOCAL: {
            uint8_t slot = READ_BYTE(frame);
            push(vm, frame->slots[slot]);
            break;
        }
        case OP_SET_LOCAL: {
            uint8_t slot = READ_BYTE(frame);
            frame->slots[slot] = peek(vm, 0);
            break;
        }
    }
}