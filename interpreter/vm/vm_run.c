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
#include "compiler.h"

static bool is_false(value val)
{
    return IS_NIL(val) || (IS_BOOL(val) && !AS_BOOL(val));
}

interpret_result run(vm_t *vm)
{
    call_frame *frame = &vm->frames[vm->frame_count - 1];
    for (int res = 0;; res = 0) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("          ");
    for (value *slot = vm->stack; slot < vm->stack_top; slot++) {
        printf("[ ");
        print_value(*slot);
        printf(" ]");
    }
    printf("\n");
    disassemble_instruction(&frame->closure->function->chunk,
                (int)(frame->ip - frame->closure->function->chunk.code));
#endif
        uint8_t instruction;
        switch (instruction = READ_BYTE(frame)) {
            case OP_CONST:
            case OP_NIL:
            case OP_TRUE:
            case OP_FALSE:
            case OP_POP:
                value_instruction(vm, frame, instruction);
                break;
            case OP_GET_LOCAL:
            case OP_SET_LOCAL:
                local_instruction(vm, frame, instruction);
                break;
            case OP_GET_GLOBAL:
            case OP_DEFINE_GLOBAL:
            case OP_SET_GLOBAL:
                res = global_instruction(vm, frame, instruction);
                if (res == INTERPRET_RUNTIME_ERROR)
                    return res;
                break;
            case OP_GET_UPVALUE:
            case OP_SET_UPVALUE:
                upvalue_instruction(vm, frame, instruction);
                break;
            case OP_GET_PROPERTY:
            case OP_SET_PROPERTY:
                res = property_instruction(vm, frame, instruction);
                if (res == INTERPRET_RUNTIME_ERROR)
                    return res;
                break;
            case OP_GET_SUPER: {
                obj_string *name = READ_STRING(frame);
                obj_class *superclass = AS_CLASS(pop(vm));
                if (!bind_method(vm, superclass, name))
                    return INTERPRET_RUNTIME_ERROR;
                break;
            }
            case OP_EQUAL: {
                value b = pop(vm);
                value a = pop(vm);
                push(vm, BOOL_VAL(values_equal(a, b)));
                break;
            }
            case OP_GREATER:
                BINARY_OP(vm, BOOL_VAL, >);
                break;
            case OP_LESS:
                BINARY_OP(vm, BOOL_VAL, <);
                break;
            case OP_ADD: {
                if (IS_STRING(peek(vm, 0)) && IS_STRING(peek(vm, 1))) {
                    concatenate(vm);
                } else if (IS_NUMBER(peek(vm, 0)) && IS_NUMBER(peek(vm, 1))) {
                    double b = AS_NUMBER(pop(vm));
                    double a = AS_NUMBER(pop(vm));
                    push(vm, NUMBER_VAL(a + b));
                } else {
                    if ((IS_STRING(peek(vm, 0)) && IS_NUMBER(peek(vm, 1))) ||
                        (IS_STRING(peek(vm, 1)) && IS_NUMBER(peek(vm, 0)))) {
                        concatenate_number(vm);
                    } else {
                        runtime_error(vm,
                                "Operands must be two numbers or two strings.");
                        return INTERPRET_RUNTIME_ERROR;
                    }
                } break;
            }
            case OP_SUB: BINARY_OP(vm, NUMBER_VAL, -); break;
            case OP_MUL: BINARY_OP(vm, NUMBER_VAL, *); break;
            case OP_DIV: BINARY_OP(vm, NUMBER_VAL, /); break;
            case OP_NOT:
                push(vm, BOOL_VAL(is_false(pop(vm))));
                break;
            case OP_NEGATE: {
                if (!IS_NUMBER(peek(vm, 0))) {
                    runtime_error(vm, "Operand must be a number");
                    return (INTERPRET_RUNTIME_ERROR);
                }
                push(vm, NUMBER_VAL(-AS_NUMBER(pop(vm))));
                break;
            }
            case OP_PRINT: {
                print_value(pop(vm));
                printf("\n");
                break;
            }
            case OP_JUMP: {
                uint16_t offset = READ_SHORT(frame);
                frame->ip += offset;
                break;
            }
            case OP_JUMP_IF_FALSE: {
                uint16_t offset = READ_SHORT(frame);
                if (is_false(peek(vm, 0)))
                    frame->ip += offset;
                break;
            }
            case OP_LOOP: {
                uint16_t offset = READ_SHORT(frame);
                frame->ip -= offset;
                break;
            }
            case OP_IMPORT: {
                obj_string *file_name = READ_STRING(frame);

                // If we have imported this file already, skip.
                if (!table_set(vm, &vm->imports, file_name, NIL_VAL))
                    break;
                char *s = readfile(file_name->chars);

                obj_function *function = compile(vm, s);
                if (function == NULL)
                    return INTERPRET_COMPILE_ERROR;
                push(vm, OBJ_VAL(function));
                obj_closure *closure = new_closure(vm, function);
                pop(vm);
                call(vm, closure, 0);
                frame = &vm->frames[vm->frame_count - 1];
                free(s);
                break;
            }
            case OP_CALL: {
                int arg_count = READ_BYTE(frame);
                if (!call_value(vm, peek(vm, arg_count), arg_count)) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                frame = &vm->frames[vm->frame_count - 1];
                break;
            }
            case OP_INVOKE: {
                obj_string *method = READ_STRING(frame);
                int arg_count = READ_BYTE(frame);
                if (!invoke(vm, method, arg_count)) {
                    return INTERPRET_RUNTIME_ERROR;
                }
                frame = &vm->frames[vm->frame_count - 1];
                break;
            }
            case OP_SUPER_INVOKE: {
                obj_string *method = READ_STRING(frame);
                int arg_count = READ_BYTE(frame);
                obj_class *superclass = AS_CLASS(pop(vm));
                if (!invoke_from_class(vm, superclass, method, arg_count))
                    return INTERPRET_RUNTIME_ERROR;
                frame = &vm->frames[vm->frame_count - 1];
                break;
            }
            case OP_CLOSURE: {
                obj_function *function = AS_FUNCTION(READ_CONSTANT(frame));
                obj_closure *closure = new_closure(vm, function);
                push(vm, OBJ_VAL(closure));
                for (int i = 0; i < closure->upvalue_count; i++) {
                    uint8_t is_local = READ_BYTE(frame);
                    uint8_t index = READ_BYTE(frame);
                    if (is_local)
                        closure->upvalues[i] = capture_upvalue(vm,
                                                frame->slots + index);
                    else
                        closure->upvalues[i] = frame->closure->upvalues[index];
                }
                break;
            }
            case OP_CLOSE_UPVALUE:
                close_upvalues(vm, vm->stack_top - 1);
                pop(vm);
                break;
            case OP_RETURN: {
                value result = pop(vm);
                close_upvalues(vm, frame->slots);
                vm->frame_count--;
                if (vm->frame_count == 0) {
                    pop(vm);
                    return INTERPRET_OK;
                }
                vm->stack_top = frame->slots;
                push(vm, result);
                frame = &vm->frames[vm->frame_count - 1];
                break;
            }
            case OP_CLASS:
                push(vm, OBJ_VAL(new_class(vm, READ_STRING(frame))));
                break;
            case OP_INHERIT: {
                value supclass = peek(vm, 1);
                if (!IS_CLASS(supclass)) {
                    runtime_error(vm, "Superclass must be a class.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                obj_class *subclass = AS_CLASS(peek(vm, 0));
                table_add_all(vm, &AS_CLASS(supclass)->methods,
                                            &subclass->methods);
                pop(vm); // Subclass
                break;
            }
            case OP_METHOD:
                define_method(vm, READ_STRING(frame));
                break;
        }
    }
}