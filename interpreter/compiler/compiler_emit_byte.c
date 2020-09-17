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

void emit_byte(parser_t *parser, uint8_t byte)
{
    write_chunk(parser->vm, current_chunk(parser),
                        byte, parser->previous.line);
}

void emit_bytes(parser_t *parser, uint8_t byte1, uint8_t byte2)
{
    emit_byte(parser, byte1);
    emit_byte(parser, byte2);
}