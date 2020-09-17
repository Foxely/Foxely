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

static token_t token_symbol_3(scanner_t *scanner, char c)
{
    switch (c) {
        case '<':
            return make_token(scanner,
                    match_tok(scanner, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return make_token(scanner,
                    match_tok(scanner, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        case '"':
            return string_tok(scanner);
    }
    return ((token_t){0});
}

static token_t token_symbol_2(scanner_t *scanner, char c)
{
    switch (c) {
        case '+':
            return make_token(scanner, TOKEN_PLUS);
        case '/':
            return make_token(scanner, TOKEN_SLASH);
        case '*':
            return make_token(scanner, TOKEN_STAR);
        case '!':
            return make_token(scanner,
                    match_tok(scanner, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return make_token(scanner,
                    match_tok(scanner, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        default:
            return token_symbol_3(scanner, c);
    }
}

token_t token_symbol(scanner_t *scanner, char c)
{
    switch (c) {
        case '(':
            return make_token(scanner, TOKEN_LEFT_PAREN);
        case ')':
            return make_token(scanner, TOKEN_RIGHT_PAREN);
        case '{':
            return make_token(scanner, TOKEN_LEFT_BRACE);
        case '}':
            return make_token(scanner, TOKEN_RIGHT_BRACE);
        case ';':
            return make_token(scanner, TOKEN_SEMICOLON);
        case ',':
            return make_token(scanner, TOKEN_COMMA);
        case '.':
            return make_token(scanner, TOKEN_DOT);
        case '-':
            return make_token(scanner, TOKEN_MINUS);
        default:
            return (token_symbol_2(scanner, c));
    }
}

token_t scan_token(scanner_t *scanner)
{
    skip_whitespace(scanner);
    scanner->start = scanner->current;

    if (is_at_end(scanner))
        return make_token(scanner, TOKEN_EOF);
    char c = advance_tok(scanner);
    if (my_char_isnum(c))
        return number_tok(scanner);
    if (my_char_isalpha(c) || c == '_')
        return identifier(scanner);
    token_t tok = token_symbol(scanner, c);
    if (tok.length != 0)
        return (tok);
    return error_token(scanner, "Unexpected character.");
}