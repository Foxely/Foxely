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

void while_statement(parser_t *parser, scanner_t *scanner)
{
    int loop_start = current_chunk(parser)->count;
    consume(parser, scanner, TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
    expression(parser, scanner);
    consume(parser, scanner, TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

    int exit_jump = emit_jump(parser, OP_JUMP_IF_FALSE);
    emit_byte(parser, OP_POP);
    statement(parser, scanner);
    emit_loop(parser, loop_start);
    patch_jump(parser, exit_jump);
    emit_byte(parser, OP_POP);
}

void for_loop(parser_t *parser, scanner_t *scan, int *exit_jump)
{
    expression(parser, scan);
    consume(parser, scan, TOKEN_SEMICOLON, "Expect ';' after loop condition.");

    *exit_jump = emit_jump(parser, OP_JUMP_IF_FALSE);
    emit_byte(parser, OP_POP);
}

void for_increment(parser_t *parser, scanner_t *scan, int *loop_start)
{
    int body_jump = emit_jump(parser, OP_JUMP);
    int increment_start = current_chunk(parser)->count;

    expression(parser, scan);
    emit_byte(parser, OP_POP);
    consume(parser, scan, TOKEN_RIGHT_PAREN,
                        "Expect ')' after for clauses.");
    emit_loop(parser, *loop_start);
    *loop_start = increment_start;
    patch_jump(parser, body_jump);
}

void for_statement(parser_t *parser, scanner_t *scanner)
{
    begin_scope(parser->comp);
    consume(parser, scanner, TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");
    if (match(parser, scanner, TOKEN_SEMICOLON)) {
    } else if (match(parser, scanner, TOKEN_VAR)) {
        var_declaration(parser, scanner);
    } else {
        expression_statement(parser, scanner);
    }
    int loop_start = current_chunk(parser)->count, exit_jump = -1;
    if (!match(parser, scanner, TOKEN_SEMICOLON))
        for_loop(parser, scanner, &exit_jump);
    if (!match(parser, scanner, TOKEN_RIGHT_PAREN))
        for_increment(parser, scanner, &loop_start);
    statement(parser, scanner);
    emit_loop(parser, loop_start);
    if (exit_jump != -1) {
        patch_jump(parser, exit_jump);
        emit_byte(parser, OP_POP);
    }
    end_scope(parser, parser->comp);
}

void import_statement(parser_t *parser, scanner_t *scanner)
{
    consume(parser, scanner, TOKEN_STRING, "Expect string after import.");

    int import_constant = make_constant(parser, OBJ_VAL(copy_string(
            parser->vm,
            parser->previous.start + 1, parser->previous.length - 2)));

    consume(parser, scanner, TOKEN_SEMICOLON, "Expect ';' after import.");

    emit_bytes(parser, OP_IMPORT, import_constant);
}