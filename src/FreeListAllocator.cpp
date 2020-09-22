#include "FreeListAllocator.h"
#include "PointerMath.h"
// #include "Utils.h"  /* CalculatePaddingWithHeader */
// #include <stdlib.h>     /* malloc, free */
#include <cassert>   /* assert		*/
// #include <limits>  /* limits_max */
// #include <algorithm>    // std::max

// #ifdef _DEBUG
// #include <iostream>
// #endif

// FreeListAllocator::FreeListAllocator(const std::size_t totalSize, const PlacementPolicy pPolicy)
// : Allocator(totalSize) {
//     m_pPolicy = pPolicy;
// }

// void FreeListAllocator::Init() {
//     if (m_start_ptr != nullptr) {
//         free(m_start_ptr);
//         m_start_ptr = nullptr;
//     }
//     m_start_ptr = malloc(m_totalSize);

//     this->Reset();
// }

// FreeListAllocator::~FreeListAllocator() {
//     free(m_start_ptr);
//     m_start_ptr = nullptr;
// }

// void* FreeListAllocator::Allocate(const std::size_t size, const std::size_t alignment) {
//     const std::size_t allocationHeaderSize = sizeof(FreeListAllocator::AllocationHeader);
//     const std::size_t freeHeaderSize = sizeof(FreeListAllocator::FreeHeader);
//     assert("Allocation size must be bigger" && size >= sizeof(Node));
//     assert("Alignment must be 8 at least" && alignment >= 8);

//     // Search through the free list for a free block that has enough space to allocate our data
//     std::size_t padding;
//     Node * affectedNode, 
//          * previousNode;
//     this->Find(size, alignment, padding, previousNode, affectedNode);
//     assert (affectedNode != nullptr && "Not enough memory");


//     const std::size_t alignmentPadding =  padding - allocationHeaderSize;
//     const std::size_t requiredSize = size + padding;    

//     const std::size_t rest = affectedNode->data.blockSize - requiredSize;

//     if (rest > 0) {
//         // We have to split the block into the data block and a free block of size 'rest'
//         Node * newFreeNode = (Node *)((std::size_t) affectedNode + requiredSize);
//         newFreeNode->data.blockSize = rest;
//         m_freeList.insert(affectedNode, newFreeNode);
//     }
//     m_freeList.remove(previousNode, affectedNode);

//     // Setup data block
//     const std::size_t headerAddress = (std::size_t) affectedNode + alignmentPadding;
//     const std::size_t dataAddress = headerAddress + allocationHeaderSize;
//     ((FreeListAllocator::AllocationHeader *) headerAddress)->blockSize = requiredSize;
//     ((FreeListAllocator::AllocationHeader *) headerAddress)->padding = alignmentPadding;

//     m_used += requiredSize;
//     m_peak = std::max(m_peak, m_used);

// #ifdef _DEBUG
//     std::cout << "A" << "\t@H " << (void*) headerAddress << "\tD@ " <<(void*) dataAddress << "\tS " << ((FreeListAllocator::AllocationHeader *) headerAddress)->blockSize <<  "\tAP " << alignmentPadding << "\tP " << padding << "\tM " << m_used << "\tR " << rest << std::endl;
// #endif

//     return (void*) dataAddress;
// }

// void FreeListAllocator::Find(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node *& previousNode, Node *& foundNode) {
//     switch (m_pPolicy) {
//         case FIND_FIRST:
//             FindFirst(size, alignment, padding, previousNode, foundNode);
//             break;
//         case FIND_BEST:
//             FindBest(size, alignment, padding, previousNode, foundNode);
//             break;
//     }
// }

// void FreeListAllocator::FindFirst(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node *& previousNode, Node *& foundNode) {
//     //Iterate list and return the first free block with a size >= than given size
//     Node * it = m_freeList.head,
//          * itPrev = nullptr;
    
//     while (it != nullptr) {
//         padding = Utils::CalculatePaddingWithHeader((std::size_t)it, alignment, sizeof (FreeListAllocator::AllocationHeader));
//         const std::size_t requiredSpace = size + padding;
//         if (it->data.blockSize >= requiredSpace) {
//             break;
//         }
//         itPrev = it;
//         it = it->next;
//     }
//     previousNode = itPrev;
//     foundNode = it;
// }

// void FreeListAllocator::FindBest(const std::size_t size, const std::size_t alignment, std::size_t& padding, Node *& previousNode, Node *& foundNode) {
//     // Iterate WHOLE list keeping a pointer to the best fit
//     std::size_t smallestDiff = std::numeric_limits<std::size_t>::max();
//     Node * bestBlock = nullptr;
//     Node * it = m_freeList.head,
//          * itPrev = nullptr;
//     while (it != nullptr) {
//         padding = Utils::CalculatePaddingWithHeader((std::size_t)it, alignment, sizeof (FreeListAllocator::AllocationHeader));
//         const std::size_t requiredSpace = size + padding;
//         if (it->data.blockSize >= requiredSpace && (it->data.blockSize - requiredSpace < smallestDiff)) {
//             bestBlock = it;
//         }
//         itPrev = it;
//         it = it->next;
//     }
//     previousNode = itPrev;
//     foundNode = bestBlock;
// }

// void FreeListAllocator::Free(void* ptr) {
//     // Insert it in a sorted position by the address number
//     const std::size_t currentAddress = (std::size_t) ptr;
//     const std::size_t headerAddress = currentAddress - sizeof (FreeListAllocator::AllocationHeader);
//     const FreeListAllocator::AllocationHeader * allocationHeader{ (FreeListAllocator::AllocationHeader *) headerAddress};

//     Node * freeNode = (Node *) (headerAddress);
//     freeNode->data.blockSize = allocationHeader->blockSize + allocationHeader->padding;
//     freeNode->next = nullptr;

//     Node * it = m_freeList.head;
//     Node * itPrev = nullptr;
//     while (it != nullptr) {
//         if (ptr < it) {
//             m_freeList.insert(itPrev, freeNode);
//             break;
//         }
//         itPrev = it;
//         it = it->next;
//     }
    
//     m_used -= freeNode->data.blockSize;

//     // Merge contiguous nodes
//     Coalescence(itPrev, freeNode);  

// #ifdef _DEBUG
//     std::cout << "F" << "\t@ptr " <<  ptr <<"\tH@ " << (void*) freeNode << "\tS " << freeNode->data.blockSize << "\tM " << m_used << std::endl;
// #endif
// }

// void FreeListAllocator::Coalescence(Node* previousNode, Node * freeNode) {   
//     if (freeNode->next != nullptr && 
//             (std::size_t) freeNode + freeNode->data.blockSize == (std::size_t) freeNode->next) {
//         freeNode->data.blockSize += freeNode->next->data.blockSize;
//         m_freeList.remove(freeNode, freeNode->next);
// #ifdef _DEBUG
//     std::cout << "\tMerging(n) " << (void*) freeNode << " & " << (void*) freeNode->next << "\tS " << freeNode->data.blockSize << std::endl;
// #endif
//     }
    
//     if (previousNode != nullptr &&
//             (std::size_t) previousNode + previousNode->data.blockSize == (std::size_t) freeNode) {
//         previousNode->data.blockSize += freeNode->data.blockSize;
//         m_freeList.remove(previousNode, freeNode);
// #ifdef _DEBUG
//     std::cout << "\tMerging(p) " << (void*) previousNode << " & " << (void*) freeNode << "\tS " << previousNode->data.blockSize << std::endl;
// #endif
//     }
// }

// void FreeListAllocator::Reset() {
//     m_used = 0;
//     m_peak = 0;
//     Node * firstNode = (Node *) m_start_ptr;
//     firstNode->data.blockSize = m_totalSize;
//     firstNode->next = nullptr;
//     m_freeList.head = nullptr;
//     m_freeList.insert(nullptr, firstNode);
// }

static const uint32_t ALLOCATION_MAGIC_NUMBER = 0xAEF01762;

FreeListAllocator::FreeListAllocator(size_t size, void* start)
	: Allocator(size), _free_blocks((FreeBlock*)start)
{
	assert(size > sizeof(FreeBlock));

	_free_blocks->size = size;
	_free_blocks->next = nullptr;

	#if DEBUG
		_free_blocks->magic_number = ALLOCATION_MAGIC_NUMBER;
		_next_identifier           = 1;
	#endif
}

FreeListAllocator::~FreeListAllocator()
{
	//_free_blocks = nullptr;
}

void* FreeListAllocator::Allocate(size_t size, uint8_t alignment)
{
	assert(size != 0 && alignment != 0);

	FreeBlock* prev_free_block = nullptr;
	FreeBlock* free_block      = _free_blocks;

	FreeBlock* best_fit_prev       = nullptr;
	FreeBlock* best_fit            = nullptr;
	uint8_t         best_fit_adjustment = 0;
	size_t     best_fit_total_size = 0;

	//Find best fit
	while(free_block != nullptr)
	{
	#if DEBUG
		assert(free_block->magic_number == ALLOCATION_MAGIC_NUMBER);
	#endif

		//Calculate adjustment needed to keep object correctly aligned
		uint8_t adjustment = PointerMath::AlignForwardAdjustmentWithHeader<AllocationHeader>(free_block, alignment);

		size_t total_size = size + adjustment;

		//If its an exact match use this free block
		if(free_block->size == total_size)
		{
			best_fit_prev       = prev_free_block;
			best_fit            = free_block;
			best_fit_adjustment = adjustment;
			best_fit_total_size = total_size;

			break;
		}

		//If its a better fit switch
		if(free_block->size > total_size && (best_fit == nullptr || free_block->size < best_fit->size) )
		{
			best_fit_prev       = prev_free_block;
			best_fit            = free_block;
			best_fit_adjustment = adjustment;
			best_fit_total_size = total_size;
		}

		prev_free_block = free_block;
		free_block      = free_block->next;
	}

	if(best_fit == nullptr)
		return nullptr;

	//If allocations in the remaining memory will be impossible
	if(best_fit->size - best_fit_total_size <= sizeof(AllocationHeader))
	{
		//Increase allocation size instead of creating a new FreeBlock
		best_fit_total_size = best_fit->size;

		if(best_fit_prev != nullptr)
			best_fit_prev->next = best_fit->next;
		else
			_free_blocks = best_fit->next;
	}
	else
	{
		//Prevent new block from overwriting best fit block info
		assert(best_fit_total_size > sizeof(FreeBlock));

		//Else create a new FreeBlock containing remaining memory
		FreeBlock* new_block = (FreeBlock*)(PointerMath::Add(best_fit, best_fit_total_size));
		new_block->size      = best_fit->size - best_fit_total_size;
		new_block->next      = best_fit->next;

		#if DEBUG
			new_block->magic_number = ALLOCATION_MAGIC_NUMBER;
		#endif

		if(best_fit_prev != nullptr)
			best_fit_prev->next = new_block;
		else
			_free_blocks = new_block;
	}

	uintptr_t aligned_address = (uintptr_t) best_fit + best_fit_adjustment;

	AllocationHeader* header = (AllocationHeader*)(aligned_address - sizeof(AllocationHeader));
	header->size             = best_fit_total_size;
	header->adjustment       = best_fit_adjustment;

	assert(PointerMath::isAligned(header));

	#if DEBUG
		header->identifier   = _next_identifier++;
		header->magic_number = ALLOCATION_MAGIC_NUMBER;
	#endif

	_used_memory += best_fit_total_size;
	_num_allocations++;

	assert(PointerMath::AlignForwardAdjustment((void*)aligned_address, alignment) == 0);

	//-----------------------------------------------------------

	#if DEBUG
		checkFreeBlockList();
	#endif

	//-----------------------------------------------------------

	return (void*)aligned_address;
}

void FreeListAllocator::Deallocate(void* p)
{
	/*
	#if DEBUG
		checkFreeBlockList();
	#endif
	*/
	//-----------------------------------------------------------

	assert(p != nullptr);

	AllocationHeader* header = (AllocationHeader*)PointerMath::Subtract(p, sizeof(AllocationHeader));

	#if DEBUG
		size_t ident = header->identifier;
		assert(ident < _next_identifier);
		assert(header->magic_number == ALLOCATION_MAGIC_NUMBER);
	#endif

	uintptr_t   block_start = reinterpret_cast<uintptr_t>(p)-header->adjustment;
	size_t block_size  = header->size;
	uintptr_t   block_end   = block_start + block_size;

	FreeBlock* prev_free_block = nullptr;
	FreeBlock* free_block      = _free_blocks;

	while(free_block != nullptr)
	{
		if((uintptr_t)free_block >= block_end)
			break;

		prev_free_block = free_block;
		free_block      = free_block->next;
	}

	if(prev_free_block == nullptr)
	{
		prev_free_block               = (FreeBlock*)block_start;
		prev_free_block->size         = block_size;
		prev_free_block->next         = _free_blocks;

		#if DEBUG
			prev_free_block->magic_number = ALLOCATION_MAGIC_NUMBER;
		#endif

		//free_block   = _free_blocks;
		_free_blocks = prev_free_block;
	}
	else if((uintptr_t)prev_free_block + prev_free_block->size == block_start)
	{
		prev_free_block->size += block_size;
	}
	else
	{
		FreeBlock* temp       = (FreeBlock*)block_start;
		temp->size            = block_size;
		temp->next            = prev_free_block->next;

		#if DEBUG
			temp->magic_number = ALLOCATION_MAGIC_NUMBER;
		#endif

		prev_free_block->next = temp;

		prev_free_block = temp;
	}
	/*
	if(free_block != nullptr && (uintptr_t)free_block == block_end)
	{
		prev_free_block->size += free_block->size;
		prev_free_block->next = free_block->next;
	}
	*/

	assert(prev_free_block != nullptr);

	if((uintptr_t)prev_free_block + prev_free_block->size == (uintptr_t)prev_free_block->next)
	{
		prev_free_block->size += prev_free_block->next->size;
		prev_free_block->next = prev_free_block->next->next;
	}

	_num_allocations--;
	_used_memory -= block_size;

	//--------------------------------------------------
	
#if DEBUG
	checkFreeBlockList();
#endif
}

#if DEBUG
void FreeListAllocator::checkFreeBlockList()
{
	//Make sure that free block list is correctly ordered
	FreeBlock* free_block = _free_blocks;

	while(free_block != nullptr)
	{
		assert(free_block->next == nullptr || free_block < free_block->next);
		assert(free_block->magic_number == ALLOCATION_MAGIC_NUMBER);

		free_block = free_block->next;
	}
}
#endif