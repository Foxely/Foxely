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

bool slash(scanner_t *scanner)
{
    if (peek_next(scanner) == '/') {
        while (peek_scan(scanner) != '\n' && !is_at_end(scanner))
            advance_tok(scanner);
    } else
        return (true);
    return (false);
}

bool sheebang(scanner_t *scanner)
{
    if (peek_next(scanner) == '!') {
        while (peek_scan(scanner) != '\n' && !is_at_end(scanner)) {
            advance_tok(scanner);
        }
    } else
        return (true);
    return (false);
}

void skip_whitespace(scanner_t *scanner)
{
    for (;;) {
        char c = peek_scan(scanner);
        if (c == ' ' || c == '\r' || c == '\t')
            advance_tok(scanner);
        if (c == '\n') {
            scanner->line++;
            advance_tok(scanner);
        }
        if (c == '#' && sheebang(scanner))
            return;
        if (c == '/' && slash(scanner))
            return;
        if (c != ' ' && c != '\r' && c != '\t' && c != '\n' &&
            c != '#' && c != '/')
            break;
    }
}