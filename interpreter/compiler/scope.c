/*
** EPITECH PROJECT, 2019
** EpiProjectGen
** File description:
** main
*/

#include <stdlib.h>
#include "compiler.h"
#include "vm.h"
#include "scanner.h"
#include "my.h"
#ifdef DEBUG_PRINT_CODE
#include "chunk_debug.h"
#include "my_debug.h"
#endif

void begin_scope(compiler *comp)
{
    comp->scope_depth++;
}

void end_scope(parser_t *parser, compiler *comp)
{
    comp->scope_depth--;

    while (comp->local_count > 0 &&
        comp->locals[comp->local_count - 1].depth > comp->scope_depth) {
        if (comp->locals[comp->local_count - 1].is_captured) {
            emit_byte(parser, OP_CLOSE_UPVALUE);
        } else {
            emit_byte(parser, OP_POP);
        }
        comp->local_count--;
    }
}