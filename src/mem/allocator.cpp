#include "allocator.hpp"

namespace {
    template<typename T>
    const T& max(const T& a, const T& b) {
        return (a < b) ? b : a;
    }
    
    template<typename T, typename Compare>
    void insertion_sort(T* array, size_t count, Compare comp) {
        for (size_t i = 1; i < count; i++) {
            printf("ALLOC INIT %llu\n", array[i].Base);
            T key;
            key.Base = array[i].Base;
            key.Size = array[i].Size;
            key.Type = array[i].Type;
            printf("AFTER ASSIGN\n");
            size_t j = i;
            printf("SIZE_T %d\n", array[j - 1].Base );
            while (j > 0 && comp(key, array[j - 1])) {
                printf("WHILE\n");
                T tmp;
                tmp.Base = array[j].Base;
                tmp.Size = array[j].Size;
                tmp.Type = array[j].Type;

                array[j].Base = array[j - 1].Base;
                array[j].Size = array[j - 1].Size;
                array[j].Type = array[j - 1].Type;

                --j;
            }
            printf("after while\n");
            array[j].Base = key.Base;
            array[j].Size = key.Size;
            array[j].Type = key.Type;
        }
    }
}

void* memmove(void* dest, const void* src, size_t count)
{
    auto* d = static_cast<unsigned char*>(dest);
    auto* s = static_cast<const unsigned char*>(src);

    if (d == s || count == 0)
        return dest;

    if (d < s)
    {
        // Copy forward
        for (size_t i = 0; i < count; ++i)
            d[i] = s[i];
    }
    else
    {
        // Copy backward (handles overlap)
        for (size_t i = count; i-- > 0; )
            d[i] = s[i];
    }

    return dest;
}

void* memset(void* s, int c, size_t n)
{
    unsigned char* p = static_cast<unsigned char*>(s);
    while (n--) {
        *p++ = static_cast<unsigned char>(c);
    }
    return s;
}

template <typename T>
void ArrayDeleteElement(T array[], size_t indexToDelete, size_t& count)
{
    memmove(array + indexToDelete, array + indexToDelete + 1, sizeof(T) * (count - indexToDelete - 1));
    --count;
}

class RegionCompare
{
public:
    bool operator()(const RegionBlocks& a, const RegionBlocks& b)
    {
        printf("REGION COMPARE\n");
        if (a.Base == b.Base) {
            printf("REGION IF\n");
            return a.Size < b.Size;
        }

        printf("REGION ELSE %d | a: %ul b: %ull\n", a.Base < b.Base, a.Base, b.Base);
        return a.Base < b.Base;
    }
};

Allocator::Allocator()
    : m_BlockSize(),
      m_MemSizeBytes(),
      m_MemSize(),
      m_MemBase(nullptr)
{    
}

bool Allocator::Initialize(uint64_t blockSize, const Region regions[], size_t regionCount) 
{
    if (blockSize == 0)
        return false;

    if (regionCount == 0)
        return false;

    m_BlockSize = blockSize;
    DetermineMemoryRange(regions, regionCount);

    if (regionCount > 1024)
    {
        printf("ERROR: regionCount %llu exceeds temp buffer size (1024)\n", (unsigned long long)regionCount);
        return false;
    }

    RegionBlocks tempRegions[1024];

    for (size_t i = 0; i < regionCount; i++)
    {
        if (regions[i].Type == RegionType::Free) 
        {
            tempRegions[i].Base = ToBlockRoundUp(regions[i].Base);
            tempRegions[i].Size = regions[i].Size / blockSize;
        }
        else 
        {
            tempRegions[i].Base = ToBlock(regions[i].Base);
            tempRegions[i].Size = DivRoundUp(regions[i].Size, blockSize);
        }
        tempRegions[i].Type = regions[i].Type;

    }

    FixOverlappingRegions(tempRegions, regionCount);
    dbg_printf("AFTER FIX OVERLAPPING\n");
    bool res = InitializeImpl(tempRegions, regionCount);
    dbg_printf("AFTER RES\n");
    return res;
}

void Allocator::DetermineMemoryRange(const Region regions[], size_t regionCount)
{
    // determine where memory begins and ends
    auto* memBase = reinterpret_cast<uint8_t*>(-1);
    uint8_t* memEnd = nullptr;

    // go through list of blocks to determine 3 things: where memory begins, where it ends and biggest free region
    for (size_t i = 0; i < regionCount; i++)
    {
        if (regions[i].Base < memBase)
            memBase = reinterpret_cast<uint8_t*>(regions[i].Base);

        if (reinterpret_cast<uint8_t*>(regions[i].Base) + regions[i].Size > memEnd)
            memEnd = reinterpret_cast<uint8_t*>(regions[i].Base) + regions[i].Size;
    }

    m_MemBase = memBase;
    m_MemSizeBytes = memEnd - memBase;
    m_MemSize = m_MemSizeBytes / m_BlockSize;
}

void Allocator::FixOverlappingRegions(RegionBlocks regions[], size_t& regionCount)
{
    insertion_sort(regions, regionCount, RegionCompare());
    printf("AFTER INSERTION\n");

    for (size_t i = 0; i < regionCount; i++)
    {
        printf("BEGIN FOR\n");
        // delete 0 sized regions
        if (regions[i].Size == 0)
        {
            printf("BEFORE ARRAY DELETE\n");
            ArrayDeleteElement(regions, i, regionCount);
            printf("AFTER ARRAY DELETE\n");
            --i;
        }
        else if (i < regionCount - 1)
        {
            dbg_printf("ELSE IF\n");
            // Regions of the same type overlapping/touching? Merge them
            if (regions[i].Type == regions[i+1].Type &&
                regions[i].Base + regions[i].Size >= regions[i+1].Base)
            {
                dbg_printf("ELSE IF IF\n");
                uint64_t end = max(regions[i].Base + regions[i].Size,
                                        regions[i+1].Base + regions[i+1].Size);
                regions[i].Size = end - regions[i].Base;
                
                ArrayDeleteElement(regions, i + 1, regionCount);
                --i;
            }
            // Regions have different types, but they overlap - prioritize reserved/used blocks
            // note: blocks are already sorted by base and size
            else if (regions[i].Type != regions[i+1].Type &&
                     regions[i].Base + regions[i].Size > regions[i+1].Base)
            {
                dbg_printf("ELSE IF ELSE IF\n");
                

                uint64_t overlapSize = regions[i].Base + regions[i].Size -
                                       regions[i+1].Base;
                    
                // reserved blocks have priority
                if (regions[i].Type != RegionType::Free)
                {
                    // give overlapping space to current block
                    if (overlapSize < regions[i+1].Size)
                    {   
                        regions[i+1].Base = regions[i+1].Base + overlapSize;
                        regions[i+1].Size -= overlapSize;
                    }
                    // current block completely overlaps next one... just remove the next one
                    else
                    {
                        ArrayDeleteElement(regions, i + 1, regionCount);
                        --i;
                    }
                }
                    
                // give overlapping space to next block
                else if (overlapSize < regions[i].Size)
                {
                    regions[i].Size -= overlapSize;
                }
   
                // next block completely overlaps current one... delete current one
                else 
                {
                    ArrayDeleteElement(regions, i, regionCount);
                    --i;
                }
            }
        }
    }

    dbg_printf("EOF\n");
}

