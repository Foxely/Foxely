/*
** EPITECH PROJECT, 2019
** EpiProjectGen
** File description:
** main
*/

#include <stdio.h>
#include <stdarg.h>
#include "vm.h"
#include "value.h"
#include "object.h"
#include "my_printf.h"

void print_pass(char *out, const char *format, va_list args)
{
    print(&out, format, args);
}

void runtime_error(vm_t *vm, const char *format, ...)
{
    va_list args;
    char out[1024];
    va_start(args, format);
    print_pass(out, format, args);
    fsprintf(2, "%s\n", out);
    for (int i = vm->frame_count - 1; i >= 0; i--) {
        call_frame *frame = &vm->frames[i];
        obj_function *function = frame->closure->function;
        size_t instruction = frame->ip - function->chunk.code - 1;
        fsprintf(2, "[line %d] in ", function->chunk.lines[instruction]);
        if (function->name == NULL) {
            fsprintf(2, "script\n");
        } else {
            fsprintf(2, "%s()\n", function->name->chars);
        }
    }
    reset_stack(vm);
}