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

void function_params(parser_t *parser, scanner_t *scanner)
{
    do {
        parser->comp->function->arity++;
        parser->comp->function->arity > 255 ? error_at_current(parser,
                            "Cannot have more than 255 parameters.") : 0;
        uint8_t param_constant = parse_variable(parser, scanner,
                                                "Expect parameter name.");
        define_variable(parser, param_constant);
    } while (match(parser, scanner, TOKEN_COMMA));
}

void
function(parser_t *parser, scanner_t *scanner, function_type type)
{
    compiler compiler;
    init_compiler(parser, &compiler, type);
    begin_scope(parser->comp);
    consume(parser, scanner, TOKEN_LEFT_PAREN,
                            "Expect '(' after function name.");
    if (!check(parser, TOKEN_RIGHT_PAREN)) {
        function_params(parser, scanner);
    }
    consume(parser, scanner, TOKEN_RIGHT_PAREN, "Expect ')' after parameters.");
    consume(parser, scanner, TOKEN_LEFT_BRACE,
                            "Expect '{' before function body.");
    block(parser, scanner);
    obj_function *function = end_compiler(parser);
    emit_bytes(parser, OP_CLOSURE, make_constant(parser, OBJ_VAL(function)));
    for (int i = 0; i < function->upvalue_count; i++) {
        emit_byte(parser, compiler.upvalues[i].is_local ? 1 : 0);
        emit_byte(parser, compiler.upvalues[i].index);
    }
}

void fun_declaration(parser_t *parser, scanner_t *scanner)
{
    uint8_t global = parse_variable(parser, scanner, "Expect function name.");
    mark_initialized(parser);
    function(parser, scanner, TYPE_FUNCTION);
    define_variable(parser, global);
}

void method(parser_t *parser, scanner_t *scanner)
{
    consume(parser, scanner, TOKEN_IDENTIFIER, "Expect method name.");
    uint8_t constant = identifier_constant(parser, &parser->previous);
    function_type type = TYPE_METHOD;
    if (parser->previous.length == 4 &&
        my_strncmp(parser->previous.start, "init", 4) == 0) {
        type = TYPE_INITIALIZER;
    }
    function(parser, scanner, type);
    emit_bytes(parser, OP_METHOD, constant);
}