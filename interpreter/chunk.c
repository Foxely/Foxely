/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#include <stdlib.h>
#include "chunk.h"
#include "custom_memory.h"

void init_chunk(chunk_t *chunk)
{
    chunk->count = 0;
    chunk->capacity = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
    init_value_array(&chunk->constants);
}

void write_chunk(vm_t *vm, chunk_t *chunk, uint8_t byte, int line)
{
    if (chunk->capacity < chunk->count + 1) {
        int old_capacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(old_capacity);
        chunk->code = GROW_ARRAY(vm, chunk->code, uint8_t,
                        old_capacity, chunk->capacity);
        chunk->lines = GROW_ARRAY(vm, chunk->lines, int,
                        old_capacity, chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

void free_chunk(vm_t *vm, chunk_t *chunk)
{
    FREE_ARRAY(vm, uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(vm, int, chunk->lines, chunk->capacity);
    free_value_array(vm, &chunk->constants);
    init_chunk(chunk);
}

int add_constant(vm_t *vm, chunk_t *chunk, value val)
{
    push(vm, val);
    write_value_array(vm, &chunk->constants, val);
    pop(vm);
    return chunk->constants.count - 1;
}