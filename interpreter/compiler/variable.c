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

void named_variable(parser_t *parser, scanner_t *scanner,
                    token_t name, bool can_assign)
{
    uint8_t get_op, set_op;
    int arg = resolve_local(parser, parser->comp, &name);
    if (arg != -1) {
        get_op = OP_GET_LOCAL;
        set_op = OP_SET_LOCAL;
    } else if ((arg = resolve_up_value(parser, parser->comp, &name)) != -1) {
        get_op = OP_GET_UPVALUE;
        set_op = OP_SET_UPVALUE;
    } else {
        arg = identifier_constant(parser, &name);
        get_op = OP_GET_GLOBAL;
        set_op = OP_SET_GLOBAL;
    }
    if (can_assign && match(parser, scanner, TOKEN_EQUAL)) {
        expression(parser, scanner);
        emit_bytes(parser, set_op, (uint8_t) arg);
    } else
        emit_bytes(parser, get_op, (uint8_t) arg);
}

void declare_variable(parser_t *parser, scanner_t *scanner)
{
    (void) scanner;
    if (parser->comp->scope_depth == 0)
        return;

    token_t *name = &parser->previous;
    for (int i = parser->comp->local_count - 1; i >= 0; i--) {
        local *loc = &parser->comp->locals[i];
        if (loc->depth != -1 && loc->depth < parser->comp->scope_depth) {
            break;
        }
        if (identifiers_equal(name, &loc->name)) {
            error(parser,
                "Variable with this name already declared in this scope.");
        }
    }
    add_local(parser, *name);
}

uint8_t
parse_variable(parser_t *parser, scanner_t *scanner, const char *msg)
{
    consume(parser, scanner, TOKEN_IDENTIFIER, msg);
    declare_variable(parser, scanner);
    if (parser->comp->scope_depth > 0)
        return (0);
    return identifier_constant(parser, &parser->previous);
}

void define_variable(parser_t *parser, uint8_t global)
{
    if (parser->comp->scope_depth > 0) {
        mark_initialized(parser);
        return;
    }
    emit_bytes(parser, OP_DEFINE_GLOBAL, global);
}

void var_declaration(parser_t *parser, scanner_t *scanner)
{
    uint8_t global = parse_variable(parser, scanner, "Expect variable name.");

    if (match(parser, scanner, TOKEN_EQUAL)) {
        expression(parser, scanner);
    } else {
        emit_byte(parser, OP_NIL);
    }
    consume(parser, scanner, TOKEN_SEMICOLON,
                "Expect ';' after variable declaration.");

    define_variable(parser, global);
}