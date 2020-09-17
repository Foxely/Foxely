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

bool check(parser_t *parser, token_type type)
{
    return parser->current.type == type;
}

bool match(parser_t *parser, scanner_t *scanner, token_type type)
{
    if (!check(parser, type))
        return false;
    advance(parser, scanner);
    return true;
}

void synchronize(parser_t *parser, scanner_t *scanner)
{
    parser->panic_mode = false;
    while (parser->current.type != TOKEN_EOF) {
        if (parser->previous.type == TOKEN_SEMICOLON)
            return;

        switch (parser->current.type) {
            case TOKEN_CLASS:
            case TOKEN_FUN:
            case TOKEN_VAR:
            case TOKEN_FOR:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_PRINT:
            case TOKEN_RETURN:
            case TOKEN_IMPORT:
                return;
            default:
                break;
        }
        advance(parser, scanner);
    }
}

uint8_t identifier_constant(parser_t *parser, token_t *name)
{
    return make_constant(parser, OBJ_VAL(
            copy_string(parser->vm, name->start, name->length)));
}

bool identifiers_equal(token_t *a, token_t *b)
{
    if (a->length != b->length)
        return false;
    return my_strncmp(a->start, b->start, a->length) == 0;
}