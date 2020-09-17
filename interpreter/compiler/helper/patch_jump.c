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

void patch_jump(parser_t *parser, int offset)
{
    int jump = current_chunk(parser)->count - offset - 2;

    if (jump > UINT16_MAX) {
        error(parser, "Too much code to jump over.");
    }

    current_chunk(parser)->code[offset] = (jump >> 8) & 0xff;
    current_chunk(parser)->code[offset + 1] = jump & 0xff;
}