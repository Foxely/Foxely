#ifndef fox_debug_h
#define fox_debug_h

#include "chunk.hpp"

void disassembleChunk(Chunk& chunk, const char* name);
int disassembleInstruction(Chunk& chunk, int offset);

#endif