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

parse_rule *get_rule(token_type type)
{
    return &rules[type];
}

void
parse_precedence(parser_t *parser, scanner_t *scan, precedence preced)
{
    advance(parser, scan);
    parse_fn prefix_rule = get_rule(parser->previous.type)->prefix;
    if (prefix_rule == NULL) {
        error(parser, "Expect expression.");
        return;
    }
    bool can_assign = preced <= PREC_ASSIGNMENT;
    prefix_rule(parser, scan, can_assign);
    while (preced <= get_rule(parser->current.type)->precedence) {
        advance(parser, scan);
        parse_fn infix_rule = get_rule(parser->previous.type)->infix;
        infix_rule(parser, scan, can_assign);
    }
    if (can_assign && match(parser, scan, TOKEN_EQUAL)) {
        error(parser, "Invalid assignment target.");
    }
}