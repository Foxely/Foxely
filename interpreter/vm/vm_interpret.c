/*
** EPITECH PROJECT, 2019
** EpiProjectGen
** File description:
** main
*/

#include <stdint.h>
#include "vm.h"
#include "compiler.h"

interpret_result interpret(vm_t *vm, const char *src)
{
    obj_function *function = compile(vm, src);
    if (function == NULL)
        return (INTERPRET_COMPILE_ERROR);
    push(vm, OBJ_VAL(function));
    obj_closure *closure = new_closure(vm, function);
    pop(vm);
    push(vm, OBJ_VAL(closure));
    call_value(vm, OBJ_VAL(closure), 0);
    return run(vm);
}