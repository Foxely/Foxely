#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef> // size_t
#include <stdint.h>
#include <cassert> //assert
#include <memory>
#include <utility>

// class Allocator {
// protected:
//     std::size_t m_totalSize;
//     std::size_t m_used;   
//     std::size_t m_peak;
// public:
//     Allocator(const std::size_t totalSize);

//     virtual ~Allocator();

//     virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) = 0;

//     virtual void Free(void* ptr) = 0;

//     virtual void Init() = 0;

//     friend class Benchmark;
// };

static const uint8_t DEFAULT_ALIGNMENT = 8;

class Allocator
{
public:

    Allocator(size_t size);

    virtual ~Allocator();

    virtual void* Allocate(size_t size, uint8_t alignment = DEFAULT_ALIGNMENT) = 0;
    virtual void Deallocate(void* p) = 0;
    size_t GetSize() const;
    size_t GetUsedMemory() const;
    size_t GetNumAllocations() const;

protected:

    size_t _size;
    size_t _used_memory;
    size_t _num_allocations;
};

template <class T, class... Args> T* New(Allocator& allocator, Args&&... args)
{
    return new (allocator.Allocate(sizeof(T), __alignof(T))) T(std::forward<Args>(args)...);
}

template <class T> T* New(Allocator& allocator)
{
    return new (allocator.Allocate(sizeof(T), __alignof(T))) T;
}

// template <class T> T* allocateNew(Allocator& allocator, const T& t)
// {
//     return new (allocator.Allocate(sizeof(T), __alignof(T))) T(t);
// }

template <class T> void Delete(Allocator& allocator, T& object)
{
    object.~T(); 
    allocator.Deallocate(&object);
}

template <class T> T* NewArray(Allocator& allocator, size_t length)
{
    assert(length != 0);

    uint8_t header_size = sizeof(size_t) / sizeof(T);

    if(sizeof(size_t) % sizeof(T) > 0)
        header_size += 1;

    //Allocate extra space to store array length in the bytes before the array
    T* p = ((T*)allocator.Allocate(sizeof(T) * (length + header_size), __alignof(T))) + header_size;

    *(((size_t *)p) - 1) = length;

    for(size_t i = 0; i < length; i++)
        new (&p[i]) T;

    return p;
}

template <class T> void DeleteArray(Allocator& allocator, T* array)
{
    assert(array != nullptr);

    size_t length = *( ((size_t *)array) - 1 );
    
    for (size_t i = 0; i < length; i++)
        array.~T();
    
    //Calculate how much extra memory was allocated to store the length before the array 
    uint8_t headerSize = sizeof(size_t)/sizeof(T);

    if(sizeof(size_t) % sizeof(T) > 0)
        headerSize += 1;

    allocator.Deallocate(array - headerSize);
}

template <class T> T* NewArrayNoConstruct(Allocator& allocator, size_t length)
{
    assert(length != 0);

    uint8_t header_size = sizeof(size_t) / sizeof(T);

    if(sizeof(size_t) % sizeof(T) > 0)
        header_size += 1;

    //Allocate extra space to store array length in the bytes before the array
    T* p = ((T*)allocator.Allocate(sizeof(T)*(length + header_size), __alignof(T))) + header_size;

    *(((size_t*)p) - 1) = length;

    return p;
}

template <class T> void DeleteArrayNoDestruct(Allocator& allocator, T* array)
{
    assert(array != nullptr);

    //Calculate how much extra memory was allocated to store the length before the array
    uint8_t header_size = sizeof(size_t) / sizeof(T);

    if(sizeof(size_t) % sizeof(T) > 0)
        header_size += 1;

    allocator.Deallocate(array - header_size);
}

#endif /* ALLOCATOR_H */