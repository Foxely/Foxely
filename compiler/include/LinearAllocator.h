#ifndef LINEARALLOCATOR_H
#define LINEARALLOCATOR_H

#include "Allocator.h"

// class LinearAllocator : public Allocator {
// protected:
// 	void* m_start_ptr = nullptr;
// 	std::size_t m_offset;
// public:
// 	LinearAllocator(const std::size_t totalSize);

// 	virtual ~LinearAllocator();

// 	virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;
	
// 	virtual void Free(void* ptr) override;

// 	virtual void Init() override;

// 	virtual void Reset();
// private:
// 	LinearAllocator(LinearAllocator &linearAllocator);
// };

class LinearAllocator : public Allocator
{
public:

	LinearAllocator(size_t size, void* start);
	~LinearAllocator();

	void* Allocate(size_t size, uint8_t alignment) override;
	//NoOp - Use rewind() or clear()
	void Deallocate(void* p) override;
	void Clear();

	void* GetStart() const;
	void* GetMark() const;

	// virtual void Rewind(void* mark) = 0;


protected:

	void* _start;
	void* _current_pos;
};

#endif /* LINEARALLOCATOR_H */