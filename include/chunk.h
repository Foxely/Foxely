/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#ifndef chunk_h
#define chunk_h

#include <stdint.h>
#include "value.h"

typedef enum {
    OP_CONST,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_POP,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_GET_GLOBAL,
    OP_DEFINE_GLOBAL,
    OP_SET_GLOBAL,
    OP_GET_UPVALUE,
    OP_SET_UPVALUE,
    OP_GET_PROPERTY,
    OP_SET_PROPERTY,
    OP_GET_SUPER,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NOT,
    OP_NEGATE,
    OP_PRINT,
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_LOOP,
    OP_IMPORT,
    OP_CALL,
    OP_INVOKE,
    OP_SUPER_INVOKE,
    OP_CLOSURE,
    OP_CLOSE_UPVALUE,
    OP_RETURN,
    OP_CLASS,
    OP_INHERIT,
    OP_METHOD,
} op_code;

typedef struct {
    int count;
    int capacity;
    uint8_t *code;
    int *lines;
    value_array constants;
} chunk_t;

void init_chunk(chunk_t *chunk);
void write_chunk(vm_t *vm, chunk_t *chunk, uint8_t byte, int line);
void free_chunk(vm_t *vm, chunk_t *chunk);
int add_constant(vm_t *vm, chunk_t *chunk, value val);

#endif