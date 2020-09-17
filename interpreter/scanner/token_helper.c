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

token_t number_tok(scanner_t *scanner)
{
    while (my_char_isnum(peek_scan(scanner)))
        advance_tok(scanner);
    if (peek_scan(scanner) == '.' && my_char_isnum(peek_next(scanner))) {
        advance_tok(scanner);
        while (my_char_isnum(peek_scan(scanner)))
            advance_tok(scanner);
    }
    return make_token(scanner, TOKEN_NUMBER);
}

token_t string_tok(scanner_t *scanner)
{
    while (peek_scan(scanner) != '"' && !is_at_end(scanner)) {
        if (peek_scan(scanner) == '\n') scanner->line++;
        advance_tok(scanner);
    }
    if (is_at_end(scanner)) return error_token(scanner, "Unterminated string.");
    advance_tok(scanner);
    return make_token(scanner, TOKEN_STRING);
}

bool match_tok(scanner_t *scanner, char expected)
{
    if (is_at_end(scanner))
        return false;
    if (*scanner->current != expected)
        return false;
    scanner->current++;
    return true;
}

char advance_tok(scanner_t *scanner)
{
    scanner->current++;
    return scanner->current[-1];
}

bool is_at_end(scanner_t *scanner)
{
    return *scanner->current == '\0';
}