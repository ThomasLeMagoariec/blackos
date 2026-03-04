#pragma once

#include <stdint.h>
#include <stddef.h>
#include "../math/math.hpp"
#include "../stdio.h"
#include "../dbg_stdio.h"

enum class RegionType
{
    Free,
    Reserved,
    Unmapped,
    Allocator,
};

using ptr_t = void*;

/**
 * Represents a memory region
 */
struct Region
{
    ptr_t Base;
    uint64_t Size;
    RegionType Type;
};

/**
 * Same as Region, but 'Base' and 'Size' are measured in "blocks"
 */
struct RegionBlocks
{
    uint64_t Base;
    uint64_t Size;
    RegionType Type;
};

void* memset(void* s, int c, size_t n);
void* memmove(void* dest, const void* src, size_t count);
void* memmove(void* dest, const void* src, size_t count);

class Allocator
{
public:
    Allocator();
    bool Initialize(uint64_t blockSize, const Region regions[], size_t regionCount);
    /**
     * Allocate a region containing at least `blocks` blocks of usable
     * memory.  The returned pointer may be passed to `Free` or
     * `Reallocate` without supplying the original size; the allocator stores
     * a small header just before the returned pointer.  This mirrors
     * standard `malloc` semantics.
     */
    ptr_t Allocate(uint32_t blocks = 1);

    /**
     * Free an allocation previously returned by `Allocate` or
     * `Reallocate`.  The `blocks` parameter is ignored and exists only for
     * backwards compatibility with older callers.
     */
    void Free(ptr_t base, uint32_t blocks = 0);

    /**
     * Resizes an existing allocation to hold `newBlocks` blocks.  If `base`
     * is null behaves like `Allocate`; if `newBlocks` is zero frees the
     * allocation and returns nullptr.  The previous size is discovered from
     * metadata written by `Allocate`, so callers need not (and should not)
     * pass it.
     */
    ptr_t Reallocate(ptr_t base, uint32_t newBlocks);

    // for statistics

protected:
    template<typename TPtr>
    inline uint64_t ToBlock(TPtr ptr)
    {
        auto* u8Ptr = reinterpret_cast<uint8_t*>(ptr);
        return (u8Ptr - m_MemBase) / m_BlockSize;
    }

    template<typename TPtr>
    inline uint64_t ToBlockRoundUp(TPtr ptr)
    {
        auto* u8Ptr = reinterpret_cast<uint8_t*>(ptr);
        return DivRoundUp(static_cast<uint64_t>(u8Ptr - m_MemBase), m_BlockSize);
    }

    inline ptr_t ToPtr(uint64_t block)
    {
        uint8_t* u8Ptr = m_MemBase + block * m_BlockSize;
        return reinterpret_cast<ptr_t>(u8Ptr);
    }

    virtual bool InitializeImpl(RegionBlocks regions[], size_t regionCount) = 0;

    // primitives used by the header-handling wrappers above.  Derived
    // allocators implement these methods and deal solely with raw block
    // counts including any header space.
    virtual ptr_t AllocateImpl(uint32_t blocks) = 0;
    virtual void FreeImpl(ptr_t base, uint32_t blocks) = 0;
    virtual ptr_t ReallocateImpl(ptr_t base, uint32_t oldBlocks, uint32_t newBlocks);

private:
    void DetermineMemoryRange(const Region regions[], size_t regionCount);
    static void FixOverlappingRegions(RegionBlocks regions[], size_t& regionCount);

    // header that we prepend to every allocation; merely records the
    // original block count requested by the caller
    struct AllocHeader { uint32_t blocks; };

    inline uint32_t HeaderBlocks() const
    {
        return (sizeof(AllocHeader) + m_BlockSize - 1) / m_BlockSize;
    }

protected:
    uint64_t m_BlockSize;
    uint64_t m_MemSizeBytes;
    uint64_t m_MemSize;

private:
    uint8_t* m_MemBase;
};
