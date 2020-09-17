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

int resolve_local(parser_t *parser, compiler *comp, token_t *name)
{
    for (int i = comp->local_count - 1; i >= 0; i--) {
        local* loc = &comp->locals[i];
        if (identifiers_equal(name, &loc->name)) {
            loc->depth == -1 ? error(parser,
                    "Cannot read local variable in its own initializer.") : 0;
            return i;
        }
    }
    return -1;
}

int add_upvalue(parser_t *parser, compiler *comp,
                        uint8_t index, bool is_local)
{
    int upvalue_count = comp->function->upvalue_count;
    for (int i = 0; i < upvalue_count; i++) {
        upvalue *up = &comp->upvalues[i];
        if (up->index == index && up->is_local == is_local) {
            return i;
        }
    }
    if (upvalue_count == UINT8_COUNT) {
        error(parser, "Too many closure variables in function.");
        return 0;
    }
    comp->upvalues[upvalue_count].is_local = is_local;
    comp->upvalues[upvalue_count].index = index;
    return comp->function->upvalue_count++;
}

int resolve_up_value(parser_t *parser, compiler *comp, token_t *name)
{
    if (comp->enclosing == NULL)
        return -1;
    int local = resolve_local(parser, comp->enclosing, name);
    if (local != -1) {
        comp->enclosing->locals[local].is_captured = true;
        return (add_upvalue(parser, comp, (uint8_t) local, true));
    }
    int upvalue = resolve_up_value(parser, comp->enclosing, name);
    if (upvalue != -1) {
        return add_upvalue(parser, comp, (uint8_t)upvalue, false);
    }
    return (-1);
}

void add_local(parser_t *parser, token_t name)
{
    if (parser->comp->local_count == UINT8_COUNT) {
        error(parser, "Too many local variables in function.");
        return;
    }
    local *loc = &parser->comp->locals[parser->comp->local_count++];
    loc->name = name;
    loc->depth = -1;
    loc->is_captured = false;
}