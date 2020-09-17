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

void emit_return(parser_t *parser)
{
    if (parser->comp->type == TYPE_INITIALIZER)
        emit_bytes(parser, OP_GET_LOCAL, 0);
    else
        emit_byte(parser, OP_NIL);
    emit_byte(parser, OP_RETURN);
}

obj_function *end_compiler(parser_t *parser)
{
    emit_return(parser);
    obj_function *func = parser->comp->function;
    #ifdef DEBUG_PRINT_CODE
    if (!parser->had_error) {
        disassemble_chunk(current_chunk(parser),
            func->name != NULL ? func->name->chars : "<script>");
    }
    #endif
    parser->comp = parser->comp->enclosing;
    return (func);
}

void emit_constant(parser_t *parser, value val)
{
    emit_bytes(parser, OP_CONST, make_constant(parser, val));
}

int emit_jump(parser_t *parser, uint8_t instruction)
{
    emit_byte(parser, instruction);
    emit_byte(parser, 0xff);
    emit_byte(parser, 0xff);
    return current_chunk(parser)->count - 2;
}

void emit_loop(parser_t *parser, int loop_start)
{
    emit_byte(parser, OP_LOOP);

    int offset = current_chunk(parser)->count - loop_start + 2;
    if (offset > UINT16_MAX)
        error(parser, "Loop body too large.");

    emit_byte(parser, (offset >> 8) & 0xff);
    emit_byte(parser, offset & 0xff);
}