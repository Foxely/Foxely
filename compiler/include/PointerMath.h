
#include <stdint.h>

namespace PointerMath
{
    inline void* AlignForward(void* Address, uint8_t alignment)
    {
        return (void*)((reinterpret_cast<uintptr_t>(Address)+static_cast<uintptr_t>(alignment - 1)) & static_cast<uintptr_t>(~(alignment - 1)));
    }

    inline uint8_t AlignForwardAdjustment(const void* Address, uint8_t alignment)
    { 
        uint8_t adjustment = alignment - (reinterpret_cast<uintptr_t>(Address) & static_cast<uintptr_t>(alignment-1));

        if(adjustment == alignment)
            return 0;

        //already aligned
        return adjustment;
    }

    inline bool isAligned(const void* address, uint8_t alignment)
    {
        return AlignForwardAdjustment(address, alignment) == 0;
    }

    template<class T>
    inline bool isAligned(const T* address)
    {
        return AlignForwardAdjustment(address, __alignof(T)) == 0;
    }

    inline void* Add(void* p, size_t x)
    {
        return (void *)(reinterpret_cast<uintptr_t>(p) + x);
    }

    inline const void* Add(const void* p, size_t x)
    {
        return (const void *)(reinterpret_cast<uintptr_t>(p) + x);
    }

    inline void* Subtract(void* p, size_t x)
    {
        return (void *)(reinterpret_cast<uintptr_t>(p) - x);
    }

    inline const void* Subtract(const void* p, size_t x)
    {
        return (const void *)(reinterpret_cast<uintptr_t>(p) - x);
    }

    template<typename T>
    inline uint8_t AlignForwardAdjustmentWithHeader(const void* pAddress, uint8_t uiAlignment) 
    {
        if(__alignof(T) > uiAlignment)
            uiAlignment = __alignof(T);

        uint8_t adjustment = sizeof(T) + AlignForwardAdjustment(PointerMath::Add(pAddress, sizeof(T)), uiAlignment);

        return adjustment;
    }
}