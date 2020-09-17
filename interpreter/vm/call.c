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

bool call(vm_t *vm, obj_closure *closure, int arg_count)
{
    if (arg_count != closure->function->arity) {
        runtime_error(vm, "Expected %d arguments but got %d.",
                                    closure->function->arity, arg_count);
        return false;
    }
    if (vm->frame_count == FRAMES_MAX) {
        runtime_error(vm, "Stack overflow.");
        return false;
    }
    call_frame *frame = &vm->frames[vm->frame_count++];
    frame->closure = closure;
    frame->ip = closure->function->chunk.code;

    frame->slots = vm->stack_top - arg_count - 1;
    return true;
}

bool call_value(vm_t *vm, value callee, int arg_count)
{
    if (IS_OBJ(callee)) {
        switch (OBJ_TYPE(callee)) {
        case OBJ_BOUND_METHOD: {
            obj_bound_method *bound = AS_BOUND_METHOD(callee);
            vm->stack_top[-arg_count - 1] = bound->receiver;
            return call(vm, bound->method, arg_count);
        }
        case OBJ_CLASS: {
            obj_class *klass = AS_CLASS(callee);
            vm->stack_top[-arg_count - 1] = OBJ_VAL(new_instance(vm, klass));
            value initializer;
            if (table_get(&klass->methods, vm->init_string, &initializer)) {
                return call(vm, AS_CLOSURE(initializer), arg_count);
            } else if (arg_count != 0) {
                runtime_error(vm,
                        "Expected 0 arguments but got %d.", arg_count);
                return false;
            }
            return true;
        }
        case OBJ_CLOSURE:
            return call(vm, AS_CLOSURE(callee), arg_count);
        case OBJ_NATIVE: {
            native_fn native = AS_NATIVE(callee);
            value result = native(vm, arg_count, vm->stack_top - arg_count);
            vm->stack_top -= arg_count + 1;
            push(vm, result);
            return true;
        }
        default:
            break;
        }
    }
    runtime_error(vm, "Can only call functions and classes.");
    return false;
}