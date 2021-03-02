
#ifndef FOX_DEBUG_HPP_
#define FOX_DEBUG_HPP_

#include "chunk.hpp"

void disassembleChunk(Chunk& chunk, const char* name);
int disassembleInstruction(Chunk& chunk, int offset);

#endif