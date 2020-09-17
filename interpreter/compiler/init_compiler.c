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

void
init_compiler(parser_t *parser, compiler *comp, function_type type)
{
    compiler temp = {
        .enclosing = parser->comp, .function = 0, .type = type,
        .local_count = 0, .scope_depth = 0
    };
    my_memcpy(comp, &temp, sizeof(compiler));
    comp->function = new_function(parser->vm);
    parser->comp = comp;
    if (type != TYPE_SCRIPT) {
        parser->comp->function->name = copy_string(parser->vm,
                                            parser->previous.start,
                                            parser->previous.length);
    }
    local *loc = &parser->comp->locals[parser->comp->local_count++];
    loc->depth = 0;
    loc->is_captured = false;
    synthetic_token("this");
    if (type != TYPE_FUNCTION) {
        loc->name = synthetic_token("this");
    } else
        loc->name = synthetic_token("");
}