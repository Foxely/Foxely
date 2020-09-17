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

void error_at(parser_t *parser, token_t *token, const char *message)
{
    if (parser->panic_mode)
        return;
    parser->panic_mode = true;
    fsprintf(2, "[line %d] Error", token->line);
    if (token->type == TOKEN_EOF) {
        fsprintf(2, " at end");
    } else if (token->type == TOKEN_ERROR) {
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }
    fsprintf(2, ": %s\n", message);
    parser->had_error = true;
}

void error(parser_t *parser, const char *message)
{
    error_at(parser, &parser->previous, message);
}

void error_at_current(parser_t *parser, const char *message)
{
    error_at(parser, &parser->current, message);
}