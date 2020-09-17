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

struct d {
    scanner_t *scanner;
    token_type type;
};

static token_type check_keyword(int start, int length,
                const char *rest, struct d d)
{
    if (d.scanner->current - d.scanner->start == start + length &&
        my_strncmp(d.scanner->start + start, rest, length) == 0) {
        return d.type;
    }
    return TOKEN_IDENTIFIER;
}

static token_type identifier_type_check_2(scanner_t *sc)
{
    token_type type = TOKEN_IDENTIFIER;
    if (sc->start[0] == 'f')
        if (sc->current - sc->start > 1) {
            sc->start[1] == 'a' ?
            type = check_keyword(2, 3, "lse", (struct d){sc, TOKEN_FALSE}) : 0;
            sc->start[1] == 'o' ?
            type = check_keyword(2, 1, "r", (struct d){sc, TOKEN_FOR}) : 0;
            sc->start[1] == 'u' ?
            type = check_keyword(2, 1, "n", (struct d){sc, TOKEN_FUN}) : 0;
        }
    if (sc->start[0] == 't')
        if (sc->current - sc->start > 1) {
            sc->start[1] == 'h' ?
            type = check_keyword(2, 2, "is", (struct d){sc, TOKEN_THIS}) : 0;
            sc->start[1] == 'r' ?
            type = check_keyword(2, 2, "ue", (struct d){sc, TOKEN_TRUE}) : 0;
        }
    if (sc->start[0] == 'i')
        if (sc->current - sc->start > 1) {
            sc->start[1] == 'f' ?
            type = check_keyword(2, 0, "", (struct d){sc, TOKEN_IF}) : 0;
            sc->start[1] == 'm' ?
            type = check_keyword(2, 4, "port", (struct d){sc, TOKEN_IMPORT}): 0;
        }
    return type;
}

static token_type identifier_type_check(scanner_t *scanner)
{
    if (scanner->start[0] == 'r')
        return check_keyword(1, 5, "eturn", (struct d){scanner, TOKEN_RETURN});
    if (scanner->start[0] == 's')
        return check_keyword(1, 4, "uper", (struct d){scanner, TOKEN_SUPER});
    if (scanner->start[0] == 'v')
        return check_keyword(1, 2, "ar", (struct d){scanner, TOKEN_VAR});
    if (scanner->start[0] == 'w')
        return check_keyword(1, 4, "hile", (struct d){scanner, TOKEN_WHILE});
    return (identifier_type_check_2(scanner));
}

token_type identifier_type(scanner_t *scanner)
{
    if (scanner->start[0] == 'a')
        return check_keyword(1, 2, "nd", (struct d){scanner, TOKEN_AND});
    if (scanner->start[0] == 'c')
        return check_keyword(1, 4, "lass", (struct d){scanner, TOKEN_CLASS});
    if (scanner->start[0] == 'e')
        return check_keyword(1, 3, "lse", (struct d){scanner, TOKEN_ELSE});
    if (scanner->start[0] == 'n')
        return check_keyword(1, 2, "il", (struct d){scanner, TOKEN_NIL});
    if (scanner->start[0] == 'o')
        return check_keyword(1, 1, "r", (struct d){scanner, TOKEN_OR});
    if (scanner->start[0] == 'p')
        return check_keyword(1, 4, "rint", (struct d){scanner, TOKEN_PRINT});
    return identifier_type_check(scanner);
}

token_t identifier(scanner_t *scanner)
{
    while (my_char_isalpha(peek_scan(scanner)) ||
            my_char_isnum(peek_scan(scanner)))
        advance_tok(scanner);
    return make_token(scanner, identifier_type(scanner));
}