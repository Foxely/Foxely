#include <stdio.h>
#include <stdarg.h>
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "vm.hpp"
#include "object.hpp"

VM::VM()
{
    ResetStack();
}

void VM::ResetStack()
{
    stackTop = stack;
}

void VM::Push(Value value)
{
    *stackTop = value;
    stackTop++;
}

Value VM::Pop()
{
    stackTop--;
    return *stackTop;
}

Value VM::Peek(int distance)
{
    return stackTop[-1 - distance];
}

bool IsFalsey(Value value)
{
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

void VM::RuntimeError(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    size_t instruction = ip - (m_oChunk.m_vCode.begin() - 1);
    int line = m_oChunk.m_vLines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);

    ResetStack();
}

InterpretResult VM::interpret(const char* source)
{
    Chunk oChunk;
    // obj_function *function = Compile(m_oParser, source, &m_oChunk);
    // if (function == NULL)
        // return (INTERPRET_COMPILE_ERROR);
    // Push(OBJ_VAL(function));
    // ObjectClosure *closure = new_closure(vm, function);
    // Pop();
    // Push(OBJ_VAL(closure));
    // CallValue(OBJ_VAL(closure), 0);
    if (!Compile(m_oParser, source, &oChunk))
        return (INTERPRET_COMPILE_ERROR);
    m_oChunk = oChunk;
    ip = m_oChunk.m_vCode.begin();
    return run();
}

InterpretResult VM::run()
{
#define READ_BYTE() (*ip++)
#define READ_CONSTANT() (m_oChunk.m_oConstants.m_vValues[READ_BYTE()])

#define BINARY_OP(valueType, op) \
    do { \
      if (!IS_NUMBER(Peek(0)) || !IS_NUMBER(Peek(1))) { \
        RuntimeError("Operands must be numbers."); \
        return INTERPRET_RUNTIME_ERROR; \
      } \
      double b = AS_NUMBER(Pop()); \
      double a = AS_NUMBER(Pop()); \
      Push(valueType(a op b)); \
    } while (false)

    for (;;)
    {
#ifdef DEBUG_TRACE_EXECUTION
        printf("          ");
        for (Value* slot = stack; slot < stackTop; slot++)
        {
            printf("[ ");
            PrintValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassembleInstruction(m_oChunk, (int)(ip - m_oChunk.m_vCode.begin()));
#endif
        uint8_t instruction;
        switch (instruction = READ_BYTE())
        {
            case OP_NIL:    Push(NIL_VAL); break;
            case OP_TRUE:   Push(BOOL_VAL(true)); break;
            case OP_FALSE:  Push(BOOL_VAL(false)); break;
            case OP_EQUAL: {
                Value b = Pop();
                Value a = Pop();
                Push(BOOL_VAL(ValuesEqual(a, b)));
                break;
            }
            case OP_GREATER:  BINARY_OP(BOOL_VAL, >); break;
            case OP_LESS:     BINARY_OP(BOOL_VAL, <); break;
            case OP_ADD:
            {
                if (IS_STRING(Peek(0)) && IS_STRING(Peek(1))) {
                    Concatenate();
                } else if (IS_NUMBER(Peek(0)) && IS_NUMBER(Peek(1))) {
                    double b = AS_NUMBER(Pop());
                    double a = AS_NUMBER(Pop());
                    Push(NUMBER_VAL(a + b));
                } else {
                    RuntimeError("Operands must be two numbers or two strings.");
                    return INTERPRET_RUNTIME_ERROR;
                }
                break;
            }
            case OP_SUB:    BINARY_OP(NUMBER_VAL, -); break;
            case OP_MUL:    BINARY_OP(NUMBER_VAL, *); break;
            case OP_DIV:    BINARY_OP(NUMBER_VAL, /); break;
            case OP_NOT:
                Push(BOOL_VAL(IsFalsey(Pop())));
            break;
            case OP_NEGATE:
            {
                if (!IS_NUMBER(Peek(0))) {
                    RuntimeError("Operand must be a number.");
                    return INTERPRET_RUNTIME_ERROR;
                }

                Push(NUMBER_VAL(-AS_NUMBER(Pop())));
                break;
            }
            case OP_RETURN:
            {
                Value v = Pop();
                PrintValue(v);
                printf("\n");
                return INTERPRET_OK;
            }

            case OP_CONST:
            {
                Value constant = READ_CONSTANT();
                Push(constant);
                break;
            }
        }
    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

void VM::Concatenate()
{
    ObjectString* b = AS_STRING(Pop());
    ObjectString* a = AS_STRING(Pop());

    // int length = a->length + b->length;
    // char* chars = ALLOCATE(char, length + 1);
    // memcpy(chars, a->chars, a->length);
    // memcpy(chars + a->length, b->chars, b->length);
    // chars[length] = '\0';

    ObjectString* result = m_oParser.TakeString(a->string + b->string);
    Push(OBJ_VAL(result));
}