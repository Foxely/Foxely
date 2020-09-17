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

bool
invoke_from_class(vm_t *vm, obj_class *klass, obj_string *name, int arg_count)
{
    value method;
    if (!table_get(&klass->methods, name, &method)) {
        runtime_error(vm, "Undefined property '%s'.", name->chars);
        return false;
    }

    return call(vm, AS_CLOSURE(method), arg_count);
}

bool invoke(vm_t *vm, obj_string *name, int arg_count)
{
    value receiver = peek(vm, arg_count);
    if (!IS_INSTANCE(receiver) && !IS_LIB(receiver)) {
        runtime_error(vm, "Only instances have methods.");
        return false;
    }
    value value;
    if (IS_LIB(receiver)) {
        if (table_get(&AS_LIB(receiver)->fields, name, &value)) {
            vm->stack_top[-arg_count - 1] = value;
            return call_value(vm, value, arg_count);
        } else {
            runtime_error(vm, "The method \"%s\" doesn't exist.", name->chars);
            return false;
        }
    }
    obj_instance *instance = AS_INSTANCE(receiver);
    if (table_get(&instance->fields, name, &value)) {
        vm->stack_top[-arg_count - 1] = value;
        return call_value(vm, value, arg_count);
    }
    return invoke_from_class(vm, instance->klass, name, arg_count);
}