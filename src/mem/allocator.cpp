#include "allocator.hpp"
#include <algorithm>

void* memmove(void* dest, const void* src, std::size_t count)
{
    auto* d = static_cast<unsigned char*>(dest);
    auto* s = static_cast<const unsigned char*>(src);

    if (d == s || count == 0)
        return dest;

    if (d < s)
    {
        // Copy forward
        for (std::size_t i = 0; i < count; ++i)
            d[i] = s[i];
    }
    else
    {
        // Copy backward (handles overlap)
        for (std::size_t i = count; i-- > 0; )
            d[i] = s[i];
    }

    return dest;
}

template <typename T>
void ArrayDeleteElement(T array[], size_t indexToDelete, size_t& count)
{
    memmove(array + indexToDelete, array + indexToDelete + 1, sizeof(T) * (count - indexToDelete - 1));
    --count;
}

Allocator::Allocator():
    m_BlockSize(),
    m_MemSizeBytes(),
    m_MemSize(),
    m_MemBase(nullptr)
{

}

bool Allocator::Initialize(uint64_t blockSize, const Region *regions, size_t regionCount) {
    m_BlockSize = blockSize;
    DetermineMemoryRange(regions, regionCount);

    RegionBlocks tmpRegions[1024];

    for (size_t i = 0; i < regionCount; i++) {

        if (regions[i].Type == RegionType::Free) {
            tmpRegions[i].Base = ToBlockRoundUp(regions[i].Base);
            tmpRegions[i].Size = regions[i].Size / blockSize;
        } else {
            tmpRegions[i].Base = ToBlock(regions[i].Base);
            tmpRegions[i].Size = DivRoundUp(regions[i].Size, blockSize);
        }

        tmpRegions[i].Type = regions[i].Type;
    }

    FixOverlappingRegions(tmpRegions, regionCount);
    return InitializeImpl(tmpRegions, regionCount);
}

void Allocator::DetermineMemoryRange(const Region regions[], size_t regionCount) {
    uint8_t* memBase = reinterpret_cast<uint8_t*>(-1);
    uint8_t* memEnd = nullptr;

    for (size_t i = 0; i < regionCount; i++) {
        if (regions[i].Base < memBase)
            memBase = reinterpret_cast<uint8_t*>(regions[i].Base);

        if (reinterpret_cast<uint8_t*>(regions[i].Base) + regions[i].Size > memEnd)
            memEnd = reinterpret_cast<uint8_t*>(regions[i].Base) + regions[i].Size;
    }

    m_MemBase = memBase;
    m_MemSizeBytes = memEnd - memBase;
    m_MemSize = m_MemSizeBytes / m_BlockSize;
}

void Allocator::FixOverlappingRegions(RegionBlocks regions[], size_t& regionCount) {
    std::sort(regions, regions + regionCount, RegionCompare());

    for (size_t i = 0; i < regionCount; i++) {
        
        if (regions[i].Size == 0) {
            ArrayDeleteElement(regions, i, regionCount);
            --i;
        } else if (i < regionCount - 1) {
            if (regions[i].Type == regions[i+1].Type &&
                regions[i].Base + regions[i].Size >= regions[i+1].Size) {

                uint64_t end = std::max(regions[i].Base + regions[i].Size,
                                        regions[i+1].Base + regions[i+1].Size);
                regions[i].Size = end - regions[i].Base;

                ArrayDeleteElement(regions, i + 1, regionCount);
                --i;
            } else if (regions[i].Type != regions[i+1].Type &&
                       regions[i].Base + regions[i].Size > regions[i+1].Base) {

                uint64_t overlapSize = regions[i].Base + regions[i].Size - regions[i+1].Base;

                if (regions[i].Type != RegionType::Free) {
                    if (overlapSize < regions[i+1].Size) {
                        regions[i+1].Base = regions[i+1].Base + overlapSize;
                        regions[i+1].Size -= overlapSize;
                    } else {
                        ArrayDeleteElement(regions, i + 1, regionCount);
                        --i;
                    }
                    
                } else if (overlapSize < regions[i].Size) {
                    regions[i].Size -= overlapSize;
                } else {
                    ArrayDeleteElement(regions, i, regionCount);
                    --i;
                }
                
            }
        }

    }

}
