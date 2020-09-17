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

chunk_t *current_chunk(parser_t *parser)
{
    return &parser->comp->function->chunk;
}

void mark_initialized(parser_t *parser)
{
    if (parser->comp->scope_depth == 0)
        return;
    parser->comp->locals[parser->comp->local_count - 1].depth =
                                        parser->comp->scope_depth;
}

void advance(parser_t *parser, scanner_t *scanner)
{
    parser->previous = parser->current;
    for (;;) {
        parser->current = scan_token(scanner);
        if (parser->current.type != TOKEN_ERROR)
            break;
        error_at_current(parser, parser->current.start);
    }
}

void consume(parser_t *parser, scanner_t *scanner,
                    token_type type, const char *message)
{
    if (parser->current.type == type) {
        advance(parser, scanner);
        return;
    }
    error_at_current(parser, message);
}

obj_function *compile(vm_t *vm, const char *src)
{
    compiler comp;
    scanner_t scanner;
    parser_t parser = {0};
    parser.had_error = false;
    parser.panic_mode = false;
    parser.vm = vm;
    vm->parser = &parser;
    init_compiler(&parser, &comp, TYPE_SCRIPT);
    init_scanner(&scanner, src);
    advance(&parser, &scanner);
    while (!match(&parser, &scanner, TOKEN_EOF)) {
        declaration(&parser, &scanner);
    }
    obj_function *func = end_compiler(&parser);
    return parser.had_error ? NULL : func;
}