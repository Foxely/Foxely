#include <cstdint>
#include "chunk.hpp"

/**
 * Constructeur pour Chunk
 */
Chunk::Chunk()
{
	m_iCount = 0;
	m_iCapacity = 0;
	m_vCode = std::vector<uint8_t>();
	m_vLines = std::vector<int>();
}

/**
 * @brief Cette fonction permet d'ajouter un byte code dans notre liste de Bytes Codes
 */
void Chunk::WriteChunk(uint8_t byte, int line)
{
//   if (chunk.capacity < chunk.count + 1) {
//     int oldCapacity = chunk.capacity;
//     chunk.capacity = GROW_CAPACITY(oldCapacity);
//     chunk.code = GROW_ARRAY(uint8_t, chunk.code,
//         oldCapacity, chunk.capacity);
//   }

//   chunk.code[chunk.count] = byte;
	m_iCount++;
    m_vCode.push_back(byte);
    m_vLines.push_back(line);
}

int Chunk::AddConstant(Value value)
{
    m_oConstants.WriteValueArray(value);
    return m_oConstants.m_vValues.size() - 1;
}
