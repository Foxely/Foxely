
#include "Allocator.h"

// Allocator::Allocator(const std::size_t totalSize){
//     m_totalSize = totalSize;
//     m_used = 0;
// }

// Allocator::~Allocator(){
//     m_totalSize = 0;
// }

Allocator::Allocator(size_t size)
{
	assert(size > 0);
    _size = size;
    _used_memory = 0;
    _num_allocations = 0;
}

Allocator::~Allocator()
{
    assert(_num_allocations == 0 && _used_memory == 0); 
    _size = 0;
}

size_t Allocator::GetSize() const
{
	return _size;
}

size_t Allocator::GetUsedMemory() const
{
	return _used_memory;
}

size_t Allocator::GetNumAllocations() const
{
	return _num_allocations;
}