#ifndef fox_chunk_h
#define fox_chunk_h

#include <vector>
#include <cstdint>
#include "common.h"
#include "value.hpp"

typedef enum {
    OP_CONST,
    OP_NIL,
    OP_TRUE,
    OP_FALSE,
    OP_POP,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_GET_GLOBAL,
    OP_DEFINE_GLOBAL,
    OP_SET_GLOBAL,
    OP_GET_UPVALUE,
    OP_SET_UPVALUE,
    OP_GET_PROPERTY,
    OP_SET_PROPERTY,
    OP_GET_SUPER,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_NOT,
    OP_NEGATE,
    OP_PRINT,
    OP_JUMP,
    OP_JUMP_IF_FALSE,
    OP_LOOP,
    OP_IMPORT,
    OP_CALL,
    OP_INVOKE,
    OP_SUPER_INVOKE,
    OP_CLOSURE,
    OP_CLOSE_UPVALUE,
    OP_RETURN,
    OP_CLASS,
    OP_INHERIT,
    OP_METHOD,
    OP_INTERFACE,
    OP_INTERFACE_PROCEDURE,

    OP_SUBSCRIPT,

    // CODES for Repl Mode
    OP_PRINT_REPL,
} OpCode;

class Chunk
{
public:
    int m_iCount;
    int m_iCapacity;
    std::vector<uint8_t> m_vCode;
    ValueArray m_oConstants;
    std::vector<int> m_vLines;

    Chunk();
    void WriteChunk(uint8_t byte, int line);
    int AddConstant(Value value);
};

#endif