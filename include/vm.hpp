#ifndef fox_vm_h
#define fox_vm_h

#include "chunk.hpp"
#include "value.hpp"
#include "Parser.h"

#define STACK_MAX 256

typedef enum
{
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

class VM
{
public:
    Chunk m_oChunk;
    std::vector<uint8_t>::iterator ip;
    Value stack[STACK_MAX];
    Value* stackTop;
    Parser m_oParser;

    VM();
    InterpretResult interpret(const char* source);

    void ResetStack();
    void Push(Value value);
    Value Pop();
    Value Peek(int distance);
    void RuntimeError(const char* format, ...);

    void EmitByte(uint8_t byte);

private:
    InterpretResult run();
};

#endif