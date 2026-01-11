#pragma once

#include <stdint.h>
#include "../math/math.hpp"


enum class RegionType {
    Free,
    Rserved,
    Unmapped,
    Allocator,
};

struct Region {
    void* Base;
    uint64_t Size;
    RegionType Type;
};

class Allocator {
public:
    Allocator();
    bool Initialize(uint64_t blockSize, const Region regions[], size_t regionCount);

    virtual void* Allocate(uint32_t blocks = 1) = 0;
    virtual void Free(void* base, uint32_t blocks) = 0;

protected:
    template<typename TPtr>
    inline uint64_t ToBlock(TPtr ptr) {
        uint8_t* u8Ptr = reinterpret_cast<uint8_t*>(ptr);
        return (u8Ptr - m_MemBase) / m_BlockSize;
    }

    template<typename TPtr>
    inline uint64_t ToBlockRoundUp(TPtr ptr) {
        uint8_t* u8Ptr = reinterpret_cast<uint8_t*>(ptr);
        return DivRoundUp(static_cast<uint64_t>(u8Ptr - m_MemBase), m_BlockSize);
    }

    inline void* ToPtr(uint64_t block) {
        uint8_t* u8Ptr = m_MemBase + block * m_BlockSize;
        return reinterpret_cast<void *>(u8Ptr);
    }

protected:
    uint64_t m_BlockSize;
    uint64_t m_MemSizeBytes;
    uint64_t m_MemSize;

private:
    uint8_t* m_MemBase;
};
