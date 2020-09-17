/*
** EPITECH PROJECT, 2019
** parser
** File description:
** parser & lexer
*/

#include "object.h"
#include "custom_memory.h"
#include "my.h"
#include "vm.h"

obj_class *new_class(vm_t *vm, obj_string *name)
{
    obj_class *klass = ALLOCATE_OBJ(vm, obj_class, OBJ_CLASS);
    klass->name = name;
    init_table(&klass->methods);
    return klass;
}

obj_upvalue *new_upvalue(vm_t *vm, value *slot)
{
    obj_upvalue *up = ALLOCATE_OBJ(vm, obj_upvalue, OBJ_UPVALUE);
    up->closed = NIL_VAL;
    up->location = slot;
    up->next = NULL;
    return up;
}

obj_closure *new_closure(vm_t *vm, obj_function *function)
{
    obj_upvalue **upvalues = ALLOCATE(vm, obj_upvalue *,
                                    function->upvalue_count);
    for (int i = 0; i < function->upvalue_count; i++) {
        upvalues[i] = NULL;
    }
    obj_closure *closure = ALLOCATE_OBJ(vm, obj_closure, OBJ_CLOSURE);
    closure->function = function;
    closure->upvalues = upvalues;
    closure->upvalue_count = function->upvalue_count;
    return closure;
}

obj_function *new_function(vm_t *vm)
{
    obj_function *function = ALLOCATE_OBJ(vm, obj_function, OBJ_FUNCTION);

    function->arity = 0;
    function->upvalue_count = 0;
    function->name = NULL;
    init_chunk(&function->chunk);
    return function;
}

void print_function(obj_function *function)
{
    if (function->name == NULL) {
        printf("<script>");
        return;
    }
    printf("<fn %s>", function->name->chars);
}