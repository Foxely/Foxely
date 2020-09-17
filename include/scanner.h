/*
** EPITECH PROJECT, 2019
** mylist
** File description:
** Header Struct Container
*/

#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <limits.h>
#include "debug.h"
#include "vm.h"
#include "common.h"

#define ELSE_IF else if

typedef enum {
    // Single-character tokens.
    TOKEN_LEFT_PAREN, TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE,
    TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS, TOKEN_PLUS,
    TOKEN_SEMICOLON, TOKEN_SLASH, TOKEN_STAR,

    // One or two character tokens.
    TOKEN_BANG, TOKEN_BANG_EQUAL,
    TOKEN_EQUAL, TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER, TOKEN_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL,

    // Literals.
    TOKEN_IDENTIFIER, TOKEN_STRING, TOKEN_NUMBER,

    // Keywords.
    TOKEN_AND, TOKEN_CLASS, TOKEN_ELSE, TOKEN_FALSE,
    TOKEN_FOR, TOKEN_FUN, TOKEN_IF, TOKEN_NIL, TOKEN_OR,
    TOKEN_PRINT, TOKEN_RETURN, TOKEN_SUPER, TOKEN_THIS,
    TOKEN_TRUE, TOKEN_VAR, TOKEN_WHILE, TOKEN_IMPORT,

    TOKEN_ERROR,
    TOKEN_EOF
} token_type;


typedef struct {
    const char *start;
    const char *current;
    int line;
} scanner_t;

typedef struct {
    token_type type;
    const char *start;
    int length;
    int line;
} token_t;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,
    PREC_OR,
    PREC_AND,
    PREC_EQUALITY,
    PREC_COMPARISON,
    PREC_TERM,
    PREC_FACTOR,
    PREC_UNARY,
    PREC_CALL,
    PREC_PRIMARY
} precedence;

typedef struct {
    token_t name;
    int depth;
    bool is_captured;
} local;

typedef struct {
    uint8_t index;
    bool is_local;
} upvalue;

typedef enum {
    TYPE_FUNCTION,
    TYPE_INITIALIZER,
    TYPE_METHOD,
    TYPE_SCRIPT
} function_type;

typedef struct compiler {
    struct compiler *enclosing;
    obj_function *function;
    function_type type;
    local locals[UINT8_COUNT];
    int local_count;
    upvalue upvalues[UINT8_COUNT];
    int scope_depth;
} compiler;

typedef struct class_compiler {
    struct class_compiler *enclosing;
    token_t name;
    bool has_superclass;
} class_compiler;

typedef struct {
    token_t current;
    token_t previous;
    bool had_error;
    bool panic_mode;
    vm_t *vm;
    compiler *comp;
    class_compiler *comp_class;
} parser_t;

typedef void (*parse_fn)(parser_t *parser, scanner_t *scan, bool can_assign);
typedef struct {
    parse_fn prefix;
    parse_fn infix;
    precedence precedence;
} parse_rule;

void init_scanner(scanner_t *scanner, const char *src);
token_t scan_token(scanner_t *scanner);
token_t make_token(scanner_t *scanner, token_type type);
void skip_whitespace(scanner_t *scanner);
char advance_tok(scanner_t *scanner);
bool match_tok(scanner_t *scanner, char expected);
token_t string_tok(scanner_t *scanner);
token_t number_tok(scanner_t *scanner);
bool is_at_end(scanner_t *scanner);
char peek_next(scanner_t *scanner);
char peek_scan(scanner_t *scanner);
token_t error_token(scanner_t *scanner, const char *message);
token_t identifier(scanner_t *scanner);
token_type identifier_type(scanner_t *scanner);

#endif