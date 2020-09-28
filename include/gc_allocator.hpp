#pragma once

#include <cstddef> // size_t
#include <stdint.h>
#include <cassert> //assert
#include <memory>
#include <utility>

// extern VM vm;

// template <class T, class... Args> T* New(Args&&... args)
// {
//     T* object = new (T(std::forward<Args>(args)...));
//     GC::Gc.AddObject(object);
//
// 	GC::Gc.bytesAllocated += sizeof(object);
//
// 	if (GC::Gc.bytesAllocated > GC::Gc.nextGC)
//     {
//         vm.AddToRoots();
//     	GC::Gc.Collect();
//     }
//     return object;
// }

// template <class T> T* New(Allocator& allocator)
// {
//     return new (allocator.Allocate(sizeof(T), __alignof(T))) T;
// }
//
// // template <class T> T* allocateNew(Allocator& allocator, const T& t)
// // {
// //     return new (allocator.Allocate(sizeof(T), __alignof(T))) T(t);
// // }
//
// template <class T> void Delete(Allocator& allocator, T& object)
// {
//     object.~T();
//     allocator.Deallocate(&object);
// }
//
// template <class T> T* NewArray(Allocator& allocator, size_t length)
// {
//     assert(length != 0);
//
//     uint8_t header_size = sizeof(size_t) / sizeof(T);
//
//     if(sizeof(size_t) % sizeof(T) > 0)
//         header_size += 1;
//
//     //Allocate extra space to store array length in the bytes before the array
//     T* p = ((T*)allocator.Allocate(sizeof(T) * (length + header_size), __alignof(T))) + header_size;
//
//     *(((size_t *)p) - 1) = length;
//
//     for(size_t i = 0; i < length; i++)
//         new (&p[i]) T;
//
//     return p;
// }
//
// template <class T> void DeleteArray(Allocator& allocator, T* array)
// {
//     assert(array != nullptr);
//
//     size_t length = *( ((size_t *)array) - 1 );
//
//     for (size_t i = 0; i < length; i++)
//         array.~T();
//
//     //Calculate how much extra memory was allocated to store the length before the array
//     uint8_t headerSize = sizeof(size_t)/sizeof(T);
//
//     if(sizeof(size_t) % sizeof(T) > 0)
//         headerSize += 1;
//
//     allocator.Deallocate(array - headerSize);
// }
