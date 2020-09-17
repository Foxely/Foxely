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

uint8_t argument_list(parser_t *parser, scanner_t *scan) {
    uint8_t arg_count = 0;
    if (!check(parser, TOKEN_RIGHT_PAREN)) {
        do {
            expression(parser, scan);
            arg_count == 255 ?
                error(parser, "Cannot have more than 255 arguments.") : 0;
            arg_count++;
        } while (match(parser, scan, TOKEN_COMMA));
    }
    consume(parser, scan, TOKEN_RIGHT_PAREN, "Expect ')' after arguments.");
    return arg_count;
}