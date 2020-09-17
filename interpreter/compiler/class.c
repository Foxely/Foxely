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

void class_define(parser_t *parser, scanner_t *scanner, token_t *class_name)
{
    consume(parser, scanner, TOKEN_IDENTIFIER, "Expect class name.");
    *class_name = parser->previous;
    uint8_t name_constant = identifier_constant(parser, &parser->previous);
    declare_variable(parser, scanner);
    emit_bytes(parser, OP_CLASS, name_constant);
    define_variable(parser, name_constant);
}

void sub_class(parser_t *parser, scanner_t *scanner,
                token_t *class_name, class_compiler *cl_compiler)
{
    consume(parser, scanner, TOKEN_IDENTIFIER, "Expect superclass name.");
    variable(parser, scanner, false);
    if (identifiers_equal(class_name, &parser->previous)) {
        error(parser, "A class cannot inherit from itself.");
    }
    begin_scope(parser->comp);
    add_local(parser, synthetic_token("super"));
    define_variable(parser, 0);
    named_variable(parser, scanner, *class_name, false);
    emit_byte(parser, OP_INHERIT);
    cl_compiler->has_superclass = true;
}

void class_declaration(parser_t *parser, scanner_t *scanner)
{
    token_t class_name;
    class_define(parser, scanner, &class_name);
    class_compiler cl_compiler = { .name = parser->previous,
        .has_superclass = false, .enclosing = parser->comp_class};
    parser->comp_class = &cl_compiler;
    if (match(parser, scanner, TOKEN_LESS)) {
        sub_class(parser, scanner, &class_name, &cl_compiler);
    }
    named_variable(parser, scanner, class_name, false);
    consume(parser, scanner, TOKEN_LEFT_BRACE, "Expect '{' before class body.");
    while (!check(parser, TOKEN_RIGHT_BRACE) && !check(parser, TOKEN_EOF)) {
        method(parser, scanner);
    }
    consume(parser, scanner, TOKEN_RIGHT_BRACE, "Expect '}' after class body.");
    emit_byte(parser, OP_POP);
    if (cl_compiler.has_superclass)
        end_scope(parser, parser->comp);
    parser->comp_class = parser->comp_class->enclosing;
}

void declaration(parser_t *parser, scanner_t *scanner)
{
    if (match(parser, scanner, TOKEN_CLASS)) {
        class_declaration(parser, scanner);
    } else if (match(parser, scanner, TOKEN_FUN)) {
        fun_declaration(parser, scanner);
    } else {
        if (match(parser, scanner, TOKEN_VAR)) {
            var_declaration(parser, scanner);
        } else
            statement(parser, scanner);
    }
    if (parser->panic_mode)
        synchronize(parser, scanner);
}