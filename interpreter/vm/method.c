/*
** EPITECH PROJECT, 2019
** EpiProjectGen
** File description:
** main
*/

#include <stdio.h>
#include <time.h>
#include "vm.h"
#include "chunk_debug.h"
#include "my_printf.h"
#include "object.h"
#include "my.h"

void define_method(vm_t *vm, obj_string *name)
{
    value method = peek(vm, 0);
    obj_class *klass = AS_CLASS(peek(vm, 1));
    table_set(vm, &klass->methods, name, method);
    pop(vm);
}

bool bind_method(vm_t *vm, obj_class *klass, obj_string *name)
{
    value method;
    if (!table_get(&klass->methods, name, &method)) {
        runtime_error(vm, "Undefined property '%s'.", name->chars);
        return false;
    }
    obj_bound_method *bound = new_bound_method(vm, peek(vm, 0),
                                            AS_CLOSURE(method));
    pop(vm);
    push(vm, OBJ_VAL(bound));
    return true;
}