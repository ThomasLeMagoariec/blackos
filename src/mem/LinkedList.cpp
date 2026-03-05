#include "LinkedList.hpp"
#include "../math/math.hpp"

void LinkedListRegion::Clear()
{
    this->ElementUsed = false;
}

void LinkedListRegion::Set(uint64_t base,
                           uint64_t size,
                           RegionType type,
                           LinkedListRegion* prev,
                           LinkedListRegion* next)
{
    this->Base = base;
    this->Size = size;
    this->Type = type;
    this->Prev = prev;
    this->Next = next;
    this->ElementUsed = true;
}

LinkedListAllocator::LinkedListAllocator()
    : Allocator(),
      m_First(nullptr),
      m_Last(nullptr),
      m_PoolCapacity(STATIC_POOL_SIZE),
      m_PoolUsedElements(0),
      m_FirstPool(),
      m_StaticRegionPool(),
      m_CurrentPool(&m_FirstPool),
      m_CurrentPoolNextFree(0)
{
    memset(m_StaticRegionPool, 0, sizeof(m_StaticRegionPool));
    m_FirstPool.Elements = m_StaticRegionPool;
    m_FirstPool.Size = STATIC_POOL_SIZE;
    m_FirstPool.Next = &m_FirstPool;
}

bool LinkedListAllocator::InitializeImpl(RegionBlocks regions[], size_t regionCount)
{
    m_First = nullptr;
    m_Last = nullptr;

    for (size_t i = 0; i < regionCount; i++)
    {
        LinkedListRegion* region = NewRegion();
        region->Set(regions[i].Base, regions[i].Size, regions[i].Type);

        // find insertion position
        LinkedListRegion* insertPos = FindInsertionPosition(region->Base, region->Size);
        InsertRegion(region, insertPos);
    }

    return true;
}

// low-level allocation primitive; `blocks` already includes any header
// overhead.  Returns a pointer to the beginning of the region (header
// resides at offset 0) so that the public wrapper can advance past it.
ptr_t LinkedListAllocator::AllocateImpl(uint32_t blocks)
{
    ptr_t ret = AllocateInternal(blocks, RegionType::Reserved);

    // over 80% usage => add another block pool
    if (m_PoolUsedElements >= (m_PoolCapacity * 4) / 5)
        GrowPool();

    return ret;
}

ptr_t LinkedListAllocator::AllocateInternal(uint32_t blocks, RegionType type)
{
    if (blocks == 0)
        return nullptr;

    LinkedListRegion* found = FindFreeRegion(blocks);

    // out of memory?
    if (found == nullptr)
        return nullptr;

    ptr_t ret = ToPtr(found->Base);

    // create reserved block
    if (found->Size == blocks)
    {
        found->Type = type;
    }
    else
    {
        LinkedListRegion* newRegion = NewRegion();
        newRegion->Set(found->Base, blocks, type);
        InsertRegion(newRegion, found);

        found->Base += blocks;
        found->Size -= blocks;
    }

    return ret;
}

// low-level free primitive; `basePtr` points to the start of the region and
// `blocks` is the total allocation size including header.  The parameter is
// unused because we look up the region via its base address.
void LinkedListAllocator::FreeImpl(void* basePtr, uint32_t /*blocks*/)
{
    uint64_t base = ToBlock(basePtr);
    LinkedListRegion* current = FindRegion(base);

    if (current == nullptr
        || current->Type == RegionType::Free
        || current->Base != base)
        return; // not found, or region is already free

    current->Type = RegionType::Free;

    // can we merge with the previous region?
    if (current->Prev != nullptr && current->Prev->Type == RegionType::Free)
    {
        current = current->Prev;
        current->Size += current->Next->Size;
        DeleteAndReleaseRegion(current->Next);
    }

    // can we merge with the next region
    if (current->Next != nullptr && current->Next->Type == RegionType::Free)
    {
        current->Size += current->Next->Size;
        DeleteAndReleaseRegion(current->Next);
    }

    // TODO: under 20% usage? compress and free up some pools
}

// low-level reallocation primitive; both block counts include header space.
ptr_t LinkedListAllocator::ReallocateImpl(ptr_t base, uint32_t oldBlocks, uint32_t newBlocks)
{
    // handle edge cases (note: caller wrapper handles null/newBlocks=0 but
    // repeat for safety)
    if (base == nullptr)
        return AllocateImpl(newBlocks);
    if (newBlocks == 0) {
        FreeImpl(base, oldBlocks);
        return nullptr;
    }
    if (oldBlocks == newBlocks)
        return base;

    uint64_t blockIndex = ToBlock(base);
    LinkedListRegion* region = FindRegion(blockIndex);
    if (region != nullptr) {
        // shrinking
        if (newBlocks < oldBlocks) {
            uint64_t remainder = oldBlocks - newBlocks;
            region->Size = newBlocks;
            LinkedListRegion* extra = NewRegion();
            extra->Set(blockIndex + newBlocks, remainder, RegionType::Free);
            InsertRegion(extra, region->Next);
            if (extra->Next != nullptr && extra->Next->Type == RegionType::Free) {
                extra->Size += extra->Next->Size;
                DeleteAndReleaseRegion(extra->Next);
            }
            return base;
        }

        // growing: try to extend into a following free region
        if (newBlocks > oldBlocks && region->Next != nullptr && region->Next->Type == RegionType::Free) {
            uint64_t needed = newBlocks - oldBlocks;
            if (region->Next->Size >= needed) {
                if (region->Next->Size == needed) {
                    region->Size += needed;
                    DeleteAndReleaseRegion(region->Next);
                } else {
                    region->Size += needed;
                    region->Next->Base += needed;
                    region->Next->Size -= needed;
                }
                return base;
            }
        }
    }

    // fallback: allocate new region and copy
    ptr_t newPtr = AllocateImpl(newBlocks);
    if (newPtr == nullptr)
        return nullptr;

    uint64_t toCopy = (oldBlocks < newBlocks ? oldBlocks : newBlocks) * m_BlockSize;
    if (toCopy > 0)
        memmove(newPtr, base, (size_t)toCopy);

    FreeImpl(base, oldBlocks);
    return newPtr;
}

LinkedListRegion* LinkedListAllocator::NewRegion()
{

    do {
        if (m_CurrentPoolNextFree >= m_CurrentPool->Size)
        {
            m_CurrentPool = m_CurrentPool->Next;
            m_CurrentPoolNextFree = 0;
        }
        if (m_CurrentPool == nullptr)
        {
            m_CurrentPool = &m_FirstPool;
            m_CurrentPoolNextFree = 0;
        }

    } while (m_CurrentPool->Elements[m_CurrentPoolNextFree++].ElementUsed);

    ++m_PoolUsedElements;
    return &m_CurrentPool->Elements[m_CurrentPoolNextFree - 1];
}

void LinkedListAllocator::ReleaseRegion(LinkedListRegion* region)
{
    region->Clear();
    m_PoolUsedElements--;
}

void LinkedListAllocator::GrowPool()
{
    // allocate another pool
    auto* u8NewPool = reinterpret_cast<uint8_t*>(AllocateInternal(1, RegionType::Allocator));

    auto* newPool = reinterpret_cast<LinkedListRegionPool*>(u8NewPool);
    newPool->Elements = reinterpret_cast<LinkedListRegion*>(u8NewPool + sizeof(LinkedListRegionPool));
    newPool->Size = (m_BlockSize - sizeof(LinkedListRegionPool)) / sizeof(LinkedListRegion);
    memset(newPool->Elements, 0, sizeof(LinkedListRegion) * newPool->Size);

    newPool->Next = m_FirstPool.Next;
    m_FirstPool.Next = newPool;
    m_PoolCapacity += newPool->Size;
}

LinkedListRegion* LinkedListAllocator::FindRegion(uint64_t base)
{
    LinkedListRegion* current = m_First;
    while (current != nullptr && base > current->Base)
        current = current->Next;

    if (current != nullptr && base == current->Base)
        return current;

    return nullptr;
}

LinkedListRegion* LinkedListAllocator::FindInsertionPosition(uint64_t base, size_t size)
{
    LinkedListRegion* insertPos = m_First;
    while (insertPos != nullptr && (base > insertPos->Base || (base == insertPos->Base && size > insertPos->Size)))
        insertPos = insertPos->Next;

    return insertPos;
}

void LinkedListAllocator::InsertRegion(LinkedListRegion* region, LinkedListRegion* insertBefore)
{
    // First region in list
    if (m_First == nullptr)
    {
        m_First = m_Last = region;
        region->Next = nullptr;
        region->Prev = nullptr;
    }
    // last
    else if (insertBefore == nullptr)
    {
        region->Prev = m_Last;
        region->Next = nullptr;
        m_Last->Next = region;
        m_Last = region;
    }
    // first
    else if (insertBefore->Prev == nullptr)
    {
        region->Prev = nullptr;
        region->Next = m_First;
        m_First->Prev = region;
        m_First = region;
    }
    // middle
    else
    {
        region->Prev = insertBefore->Prev;
        region->Next = insertBefore;
        insertBefore->Prev->Next = region;
        insertBefore->Prev = region;
    }
}

void LinkedListAllocator::DeleteRegion(LinkedListRegion* region)
{
    // first?
    if (region->Prev == nullptr)
        m_First = region->Next;
    else
	    region->Prev->Next = region->Next;

    // last?
    if (region->Next == nullptr)
        m_Last = region->Prev;
    else
	    region->Next->Prev = region->Prev;

}

void LinkedListAllocator::DeleteAndReleaseRegion(LinkedListRegion* region)
{
    DeleteRegion(region);
    ReleaseRegion(region);
}

// for statistics
/*
RegionType LinkedListAllocator::GetState(ptr_t address)
{
    uint64_t block = ToBlock(address);
    for (auto current = m_First; current != nullptr; current = current->Next)
    {
        if (block >= current->Base && block < current->Base + current->Size)
            return current->Type;
    }

    return RegionType::Unmapped;
}*/

// for debugging


LinkedListRegion* LinkedListAllocator::FindFreeRegion(uint32_t blocks)
{
    LinkedListRegion* current = m_First;
    while (current != nullptr && (current->Type != RegionType::Free || current->Size < blocks))
        current = current->Next;

    return current;
}


LinkedListRegion* LinkedListAllocatorFirstFit::FindFreeRegion(uint32_t blocks)
{
    LinkedListRegion* current = m_First;
    while (current != nullptr && (current->Type != RegionType::Free || current->Size < blocks))
        current = current->Next;

    return current;
}

LinkedListAllocatorNextFit::LinkedListAllocatorNextFit()
    : LinkedListAllocator(),
      m_Next(nullptr)
{
}

LinkedListRegion* LinkedListAllocatorNextFit::FindFreeRegion(uint32_t blocks)
{
    if (m_Next == nullptr)
        m_Next = m_First;

    LinkedListRegion* current = m_Next;
    while (current->Type != RegionType::Free || current->Size < blocks)
    {
        current = current->Next;

        if (current == nullptr)
            current = m_First;

        // we wrapped around and found nothing
        if (current == m_Next)
            return nullptr;
    }

    m_Next = current;
    return current;
}

void LinkedListAllocatorNextFit::DeleteRegion(LinkedListRegion* region)
{
    LinkedListAllocator::DeleteRegion(region);

    if (region == m_Next)
        m_Next = nullptr;
}


LinkedListRegion* LinkedListAllocatorBestFit::FindFreeRegion(uint32_t blocks)
{
    LinkedListRegion* found = nullptr;
    LinkedListRegion* current = m_First;

    while (current != nullptr)
    {
        if (current->Type == RegionType::Free
            && current->Size > blocks
            && (found == nullptr || current->Size < found->Size))
            found = current;

        current = current->Next;
    }

    return found;
}

LinkedListRegion* LinkedListAllocatorWorstFit::FindFreeRegion(uint32_t blocks)
{
    LinkedListRegion* found = nullptr;
    LinkedListRegion* current = m_First;

    while (current != nullptr)
    {
        if (current->Type == RegionType::Free
            && current->Size > blocks
            && (found == nullptr || current->Size > found->Size))
            found = current;

        current = current->Next;
    }

    return found;
}
