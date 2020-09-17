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

void print_statement(parser_t *parser, scanner_t *scanner)
{
    expression(parser, scanner);
    consume(parser, scanner, TOKEN_SEMICOLON, "Expect ';' after value.");
    emit_byte(parser, OP_PRINT);
}

void return_statement(parser_t *parser, scanner_t *scanner)
{
    if (parser->comp->type == TYPE_SCRIPT)
        error(parser, "Cannot return from top-level code.");
    if (match(parser, scanner, TOKEN_SEMICOLON)) {
        emit_return(parser);
    } else {
        if (parser->comp->type == TYPE_INITIALIZER) {
            error(parser, "Cannot return a value from an initializer.");
        }
        expression(parser, scanner);
        consume(parser, scanner, TOKEN_SEMICOLON, "Expect ';' after return value.");
        emit_byte(parser, OP_RETURN);
    }
}

void if_statement(parser_t *parser, scanner_t *scanner)
{
    consume(parser, scanner, TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
    expression(parser, scanner);
    consume(parser, scanner, TOKEN_RIGHT_PAREN, "Expect ')' after condition.");
    int then_jump = emit_jump(parser, OP_JUMP_IF_FALSE);
    emit_byte(parser, OP_POP);
    statement(parser, scanner);
    int else_jump = emit_jump(parser, OP_JUMP);
    patch_jump(parser, then_jump);
    emit_byte(parser, OP_POP);
    if (match(parser, scanner, TOKEN_ELSE))
        statement(parser, scanner);
    patch_jump(parser, else_jump);
}

void expression_statement(parser_t *parser, scanner_t *scanner)
{
    expression(parser, scanner);
    consume(parser, scanner, TOKEN_SEMICOLON, "Expect ';' after expression.");
    emit_byte(parser, OP_POP);
}

void statement(parser_t *parser, scanner_t *scanner)
{
    if (match(parser, scanner, TOKEN_PRINT)) {
        print_statement(parser, scanner);
    } ELSE_IF (match(parser, scanner, TOKEN_FOR)) {
        for_statement(parser, scanner);
    } ELSE_IF (match(parser, scanner, TOKEN_IF)) {
        if_statement(parser, scanner);
    } ELSE_IF (match(parser, scanner, TOKEN_RETURN)) {
        return_statement(parser, scanner);
    } ELSE_IF (match(parser, scanner, TOKEN_IMPORT)) {
        import_statement(parser, scanner);
    } ELSE_IF (match(parser, scanner, TOKEN_WHILE)) {
        while_statement(parser, scanner);
    } ELSE_IF (match(parser, scanner, TOKEN_LEFT_BRACE)) {
        begin_scope(parser->comp);
        block(parser, scanner);
        end_scope(parser, parser->comp);
    } else {
        expression_statement(parser, scanner);
    }
}