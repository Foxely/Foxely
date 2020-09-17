/*
** EPITECH PROJECT, 2019
** mylist
** File description:
** Header Struct Container
*/

#ifndef __VM_H__
#define __VM_H__

#include "chunk.h"
#include "type.h"
#include "table.h"
#include "value.h"
#include "common.h"
#include "scanner.h"

typedef value (*native_fn)(vm_t *vm, int arg_count, value *args);

#define READ_BYTE(frame) (*frame->ip++)
#define READ_CONSTANT(frame) \
    (frame->closure->function->chunk.constants.values[READ_BYTE(frame)])
#define READ_STRING(frame) AS_STRING(READ_CONSTANT(frame))
#define READ_SHORT(frame) \
    (frame->ip += 2, (uint16_t)((frame->ip[-2] << 8) | frame->ip[-1]))

#define BINARY_OP(vm, valuetype, op) \
    do { \
    if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) { \
        runtime_error(vm, "Operands must be numbers.");  \
        return (INTERPRET_RUNTIME_ERROR); \
    } \
        double b = AS_NUMBER(pop(vm)); \
        double a = AS_NUMBER(pop(vm)); \
        push(vm, valuetype(a op b)); \
    } while (false)

#define FRAMES_MAX (64)
#define VM_STACK_MAX (FRAMES_MAX * UINT8_COUNT)

typedef struct {
    obj_closure *closure;
    uint8_t *ip;
    value *slots;
} call_frame;

struct s_vm {
    call_frame frames[FRAMES_MAX];
    int frame_count;
    value stack[VM_STACK_MAX];
    value *stack_top;
    table_t globals;
    table_t strings;
    table_t imports;
    obj_string *init_string;
    obj_upvalue *open_upvalues;

    size_t bytes_allocated;
    size_t next_gc;

    obj_t *objects;
    int gray_count;
    int gray_capacity;
    obj_t **gray_stack;

    parser_t *parser;
};

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
} interpret_result;


interpret_result run_file(vm_t *vm, const char *path);
char *readfile(const char *path);



void init_vm(vm_t *vm);
void free_vm(vm_t *vm);
interpret_result interpret(vm_t *vm, const char *src);
interpret_result run(vm_t *vm);


void push(vm_t *vm, value val);
value pop(vm_t *vm);
value peek(vm_t *vm, int distance);
void reset_stack(vm_t *vm);
void runtime_error(vm_t *vm, const char *format, ...);
void free_objects();
bool call_value(vm_t *vm, value callee, int arg_count);
bool call(vm_t *vm, obj_closure *closure, int arg_count);

bool
invoke_from_class(vm_t *vm, obj_class *klass, obj_string *name, int arg_count);
bool invoke(vm_t *vm, obj_string *name, int arg_count);
void close_upvalues(vm_t *vm, value *last);
obj_upvalue *capture_upvalue(vm_t *vm, value *local);
void define_method(vm_t *vm, obj_string *name);
bool bind_method(vm_t *vm, obj_class *klass, obj_string *name);


void value_instruction(vm_t *vm, call_frame *frame, uint8_t instruction);
void local_instruction(vm_t *vm, call_frame *frame, uint8_t instruction);
interpret_result
global_instruction(vm_t *vm, call_frame *frame, uint8_t instruction);
void upvalue_instruction(vm_t *vm, call_frame *frame, uint8_t instruction);
interpret_result
property_instruction(vm_t *vm, call_frame *frame, uint8_t instruction);

#endif