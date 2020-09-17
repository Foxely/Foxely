/*
** EPITECH PROJECT, 2019
** EpiProjectGen
** File description:
** main
*/

#include <stdlib.h>
#include "compiler.h"

parse_rule rules[] = {
    { grouping, call_compiler,    PREC_CALL },       // TOKEN_LEFT_PAREN
    { NULL,     NULL,    PREC_NONE },       // TOKEN_RIGHT_PAREN
    { NULL,     NULL,    PREC_NONE },      // TOKEN_LEFT_BRACE
    { NULL,     NULL,    PREC_NONE },       // TOKEN_RIGHT_BRACE
    { NULL,     NULL,    PREC_NONE },       // TOKEN_COMMA
    { NULL,     dot,    PREC_CALL },       // TOKEN_DOT
    { unary,    binary,  PREC_TERM },       // TOKEN_MINUS
    { NULL,     binary,  PREC_TERM },       // TOKEN_PLUS
    { NULL,     NULL,    PREC_NONE },       // TOKEN_SEMICOLON
    { NULL,     binary,  PREC_FACTOR },     // TOKEN_SLASH
    { NULL,     binary,  PREC_FACTOR },     // TOKEN_STAR
    { unary,     NULL,    PREC_NONE },       // TOKEN_BANG
    { NULL,     binary,    PREC_EQUALITY },       // TOKEN_BANG_EQUAL
    { NULL,     NULL,    PREC_NONE },       // TOKEN_EQUAL
    { NULL,     binary,    PREC_EQUALITY },       // TOKEN_EQUAL_EQUAL
    { NULL,     binary,    PREC_COMPARISON },       // TOKEN_GREATER
    { NULL,     binary,    PREC_COMPARISON },       // TOKEN_GREATER_EQUAL
    { NULL,     binary,    PREC_COMPARISON },       // TOKEN_LESS
    { NULL,     binary,    PREC_COMPARISON },       // TOKEN_LESS_EQUAL
    { variable,     NULL,    PREC_NONE },       // TOKEN_IDENTIFIER
    { string,     NULL,    PREC_NONE },       // TOKEN_STRING
    { number,   NULL,    PREC_NONE },       // TOKEN_NUMBER
    { NULL,     rule_and,    PREC_NONE },       // TOKEN_AND
    { NULL,     NULL,    PREC_NONE },       // TOKEN_CLASS
    { NULL,     NULL,    PREC_NONE },       // TOKEN_ELSE
    { literal,     NULL,    PREC_NONE },       // TOKEN_FALSE
    { NULL,     NULL,    PREC_NONE },       // TOKEN_FOR
    { NULL,     NULL,    PREC_NONE },       // TOKEN_FUN
    { NULL,     NULL,    PREC_NONE },       // TOKEN_IF
    { literal,     NULL,    PREC_NONE },       // TOKEN_NIL
    { NULL,     rule_or,    PREC_NONE },       // TOKEN_OR
    { NULL,     NULL,    PREC_NONE },       // TOKEN_PRINT
    { NULL,     NULL,    PREC_NONE },       // TOKEN_RETURN
    { rule_super,     NULL,    PREC_NONE },       // TOKEN_SUPER
    { rule_this,     NULL,    PREC_NONE },       // TOKEN_THIS
    { literal,     NULL,    PREC_NONE },       // TOKEN_TRUE
    { NULL,     NULL,    PREC_NONE },       // TOKEN_VAR
    { NULL,     NULL,    PREC_NONE },       // TOKEN_WHILE
    { NULL,     NULL,    PREC_NONE },       // TOKEN_ERROR
    { NULL,     NULL,    PREC_NONE },       // TOKEN_EOF
};