/*
** EPITECH PROJECT, 2019
** mylist
** File description:
** Header Struct Container
*/

#ifndef __COMPILER_H__
#define __COMPILER_H__

#include "debug.h"
#include "vm.h"
#include "scanner.h"
#include "my.h"
#include "object.h"

void
init_compiler(parser_t *parser, compiler *comp, function_type type);
void consume(parser_t *parser, scanner_t *scanner,
                    token_type type, const char *message);
obj_function *compile(vm_t *vm, const char *src);
void mark_compiler_roots(vm_t *vm, compiler *comp);
void error_at(parser_t *parser, token_t *token, const char *message);
void error(parser_t *parser, const char *message);
void error_at_current(parser_t *parser, const char *message);
uint8_t make_constant(parser_t *parser, value val);
void emit_byte(parser_t *parser, uint8_t byte);
void emit_bytes(parser_t *parser, uint8_t byte1, uint8_t byte2);
void emit_return(parser_t *parser);
obj_function *end_compiler(parser_t *parser);
void emit_constant(parser_t *parser, value val);
int emit_jump(parser_t *parser, uint8_t instruction);
void emit_loop(parser_t *parser, int loop_start);
parse_rule *get_rule(token_type type);
void advance(parser_t *parser, scanner_t *scanner);
void parse_precedence(parser_t *parser, scanner_t *scan, precedence preced);
chunk_t *current_chunk(parser_t *parser);
uint8_t identifier_constant(parser_t *parser, token_t *name);
bool match(parser_t *parser, scanner_t *scanner, token_type type);
uint8_t argument_list(parser_t *parser, scanner_t *scan);
token_t synthetic_token(const char *text);
void named_variable(parser_t *parser, scanner_t *scanner,
                    token_t name, bool can_assign);
void define_variable(parser_t *parser, uint8_t global);
void declaration(parser_t *parser, scanner_t *scanner);
bool identifiers_equal(token_t *a, token_t *b);
void var_declaration(parser_t *parser, scanner_t *scanner);
void begin_scope(compiler *comp);
void end_scope(parser_t *parser, compiler *comp);
void method(parser_t *parser, scanner_t *scanner);
void patch_jump(parser_t *parser, int offset);
void mark_initialized(parser_t *parser);
void block(parser_t *parser, scanner_t *scanner);
bool check(parser_t *parser, token_type type);
uint8_t
parse_variable(parser_t *parser, scanner_t *scanner, const char *msg);
void synchronize(parser_t *parser, scanner_t *scanner);
void fun_declaration(parser_t *parser, scanner_t *scanner);
void add_local(parser_t *parser, token_t name);
void declare_variable(parser_t *parser, scanner_t *scanner);
int resolve_up_value(parser_t *parser, compiler *comp, token_t *name);
int resolve_local(parser_t *parser, compiler *comp, token_t *name);



void statement(parser_t *parser, scanner_t *scanner);
void expression_statement(parser_t *parser, scanner_t *scanner);
void print_statement(parser_t *parser, scanner_t *scanner);
void return_statement(parser_t *parser, scanner_t *scanner);
void import_statement(parser_t *parser, scanner_t *scanner);
void if_statement(parser_t *parser, scanner_t *scanner);
void for_statement(parser_t *parser, scanner_t *scanner);
void while_statement(parser_t *parser, scanner_t *scanner);


static inline void number(parser_t *parser, scanner_t *scanne, bool can_assign)
{
    (void) can_assign;
    (void) scanne;
    double val = my_strtod((char *) parser->previous.start, NULL);
    emit_constant(parser, NUMBER_VAL(val));
}

static inline void
variable(parser_t *parser, scanner_t *scanne, bool can_assign) {
    named_variable(parser, scanne, parser->previous, can_assign);
}

static inline void string(parser_t *parser, scanner_t *scanne, bool can_assign)
{
    (void) can_assign;
    (void) scanne;
    emit_constant(parser,
            OBJ_VAL(copy_string(parser->vm, parser->previous.start + 1,
                                    parser->previous.length - 2)));
}

static inline void expression(parser_t *parser, scanner_t *scan)
{
    parse_precedence(parser, scan, PREC_ASSIGNMENT);
}

static inline void grouping(parser_t *parser, scanner_t *scan, bool can_assign)
{
    (void) can_assign;
    expression(parser, scan);
    consume(parser, scan, TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static inline
void call_compiler(parser_t *parser, scanner_t *scan, bool can_assign) {
    (void) can_assign;
    uint8_t arg_count = argument_list(parser, scan);
    emit_bytes(parser, OP_CALL, arg_count);
}

static inline void unary(parser_t *parser, scanner_t *scanne, bool can_assign)
{
    (void) can_assign;
    token_type operator_type = parser->previous.type;
    expression(parser, scanne);
    switch (operator_type) {
        case TOKEN_BANG:
            emit_byte(parser, OP_NOT);
            break;
        case TOKEN_MINUS:
            emit_byte(parser, OP_NEGATE);
            break;
        default:
        return;
    }
}

static inline void
binary(parser_t *parser, scanner_t *scanner, bool can_assign) {
    (void) can_assign;
    token_type operatorType = parser->previous.type;
    parse_rule *rule = get_rule(operatorType);
    parse_precedence(parser, scanner, (precedence)(rule->precedence + 1));

    switch (operatorType) {
        case TOKEN_BANG_EQUAL:
            emit_bytes(parser, OP_EQUAL, OP_NOT);
            break;
        case TOKEN_EQUAL_EQUAL:
            emit_byte(parser, OP_EQUAL);
            break;
        case TOKEN_GREATER:
            emit_byte(parser, OP_GREATER);
            break;
        case TOKEN_GREATER_EQUAL:
            emit_bytes(parser, OP_LESS, OP_NOT);
            break;
        case TOKEN_LESS:
            emit_byte(parser, OP_LESS);
            break;
        case TOKEN_LESS_EQUAL:
            emit_bytes(parser, OP_GREATER, OP_NOT);
            break;
        case TOKEN_PLUS:
            emit_byte(parser, OP_ADD);
            break;
        case TOKEN_MINUS:
            emit_byte(parser, OP_SUB);
            break;
        case TOKEN_STAR:
            emit_byte(parser, OP_MUL);
        break;
        case TOKEN_SLASH:
            emit_byte(parser, OP_DIV);
        break;
        default:
            return;
    }
}

static inline void
literal(parser_t *parser, scanner_t *scanner, bool can_assign) {
    (void) scanner;
    (void) can_assign;
    switch (parser->previous.type) {
        case TOKEN_FALSE:
            emit_byte(parser, OP_FALSE);
            break;
        case TOKEN_NIL:
            emit_byte(parser, OP_NIL);
            break;
        case TOKEN_TRUE:
            emit_byte(parser, OP_TRUE);
            break;
        default:
        return;
    }
}

static inline void
rule_and(parser_t *parser, scanner_t *scanner, bool can_assign) {
    (void) can_assign;
    int end_jump = emit_jump(parser, OP_JUMP_IF_FALSE);

    emit_byte(parser, OP_POP);
    parse_precedence(parser, scanner, PREC_AND);
    patch_jump(parser, end_jump);
}

static inline void
rule_or(parser_t *parser, scanner_t *scanner, bool can_assign) {
    (void) can_assign;
    int else_jump = emit_jump(parser, OP_JUMP_IF_FALSE);
    int end_jump = emit_jump(parser, OP_JUMP);

    patch_jump(parser, else_jump);
    emit_byte(parser, OP_POP);

    parse_precedence(parser, scanner, PREC_OR);
    patch_jump(parser, end_jump);
}

static inline void dot(parser_t *parser, scanner_t *scanner, bool can_assign) {
    consume(parser, scanner, TOKEN_IDENTIFIER,
                            "Expect property name after '.'.");
    uint8_t name = identifier_constant(parser, &parser->previous);

    if (can_assign && match(parser, scanner, TOKEN_EQUAL)) {
        expression(parser, scanner);
        emit_bytes(parser, OP_SET_PROPERTY, name);
    } else if (match(parser, scanner, TOKEN_LEFT_PAREN)) {
        uint8_t arg_count = argument_list(parser, scanner);
        emit_bytes(parser, OP_INVOKE, name);
        emit_byte(parser, arg_count);
    } else {
        emit_bytes(parser, OP_GET_PROPERTY, name);
    }
}

static inline void rule_this(parser_t *parser,
                        scanner_t *scanner, bool can_assign) {
    (void) can_assign;
    if (parser->comp_class == NULL) {
        error(parser, "Cannot use 'this' outside of a class.");
        return;
    }
    variable(parser, scanner, false);
}

static inline void rule_super(parser_t *parser, scanner_t *scan, bool can) {
    (void) can;
    if (parser->comp_class == NULL) {
        error(parser, "Cannot use 'super' outside of a class.");
    } else if (!parser->comp_class->has_superclass) {
        error(parser, "Cannot use 'super' in a class with no superclass.");
    }
    consume(parser, scan, TOKEN_DOT, "Expect '.' after 'super'.");
    consume(parser, scan, TOKEN_IDENTIFIER, "Expect superclass method name.");
    uint8_t name = identifier_constant(parser, &parser->previous);
    named_variable(parser, scan, synthetic_token("this"), false);
    if (match(parser, scan, TOKEN_LEFT_PAREN)) {
        uint8_t arg_count = argument_list(parser, scan);
        named_variable(parser, scan, synthetic_token("super"), false);
        emit_bytes(parser, OP_SUPER_INVOKE, name);
        emit_byte(parser, arg_count);
    } else {
        named_variable(parser, scan, synthetic_token("super"), false);
        emit_bytes(parser, OP_GET_SUPER, name);
    }
}
extern parse_rule rules[];

#endif