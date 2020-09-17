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

char peek_next(scanner_t *scanner)
{
    if (is_at_end(scanner))
        return '\0';
    return scanner->current[1];
}

char peek_scan(scanner_t *scanner)
{
    return *scanner->current;
}

void init_scanner(scanner_t *scanner, const char *src)
{
    scanner->start = src;
    scanner->current = src;
    scanner->line = 1;
}