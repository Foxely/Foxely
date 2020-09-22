#ifndef FREELISTALLOCATOR_H
#define FREELISTALLOCATOR_H

#include "Allocator.h"
// #include "SinglyLinkedList.h"

// class FreeListAllocator : public Allocator {
// public:
//     enum PlacementPolicy {
//         FIND_FIRST,
//         FIND_BEST
//     };

// private:
//     struct FreeHeader {
//         std::size_t blockSize;
//     };
//     struct AllocationHeader {
//         std::size_t blockSize;
//         char padding;
//     };
    
//     typedef SinglyLinkedList<FreeHeader>::Node Node;

    
//     void* m_start_ptr = nullptr;
//     PlacementPolicy m_pPolicy;
//     SinglyLinkedList<FreeHeader> m_freeList;

// public:
//     FreeListAllocator(const std::size_t totalSize, const PlacementPolicy pPolicy);

//     virtual ~FreeListAllocator();

//     virtual void* Allocate(const std::size_t size, const std::size_t alignment = 0) override;

//     virtual void Free(void* ptr) override;

//     virtual void Init() override;

//     virtual void Reset();
// private:
//     FreeListAllocator(FreeListAllocator &freeListAllocator);

//     void Coalescence(Node* prevBlock, Node * freeBlock);

//     void Find(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
//     void FindBest(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
//     void FindFirst(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node*& previousNode, Node*& foundNode);
// };


class FreeListAllocator : public Allocator
{
public:
    FreeListAllocator(size_t size, void* start);
    ~FreeListAllocator();

    void* Allocate(size_t size, uint8_t alignment) override;

    void Deallocate(void* p) override;

#if DEBUG
    void checkFreeBlockList();
#endif

private:

    struct AllocationHeader
    {
        #if DEBUG
            size_t identifier;
            uint32_t magic_number;
        #endif
        size_t size;
        uint8_t adjustment;
    };

    struct FreeBlock
    {
        #if DEBUG
            uint32_t magic_number;
        #endif
        size_t     size;
        FreeBlock* next;
    };

    static_assert(sizeof(AllocationHeader) >= sizeof(FreeBlock), "sizeof(AllocationHeader) < sizeof(FreeBlock)");

    FreeListAllocator(const FreeListAllocator&);
    FreeListAllocator& operator=(const FreeListAllocator&);

    FreeBlock* _free_blocks;

    #if DEBUG
    size_t _next_identifier;
    #endif
};

#endif /* FREELISTALLOCATOR_H */
