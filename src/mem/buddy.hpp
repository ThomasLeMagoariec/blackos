#include "allocator.hpp"
#include "../math/math.hpp"

class BuddyAllocator : public Allocator {
public:
    BuddyAllocator();
    void* Allocate(uint32_t blocks = 1) override;
    void Free(void* base, uint32_t blocks) override;

protected:
    bool InitializeImpl(RegionBlocks regions[], size_t regionCount) override;

    inline uint64_t BlocksOnLayer(int layer) {
        return (1ull << layer) * m_BlocksLayer0;
    }

    inline uint64_t IndexOfLayer(int layer) {
        return DivRoundUp(m_BlocksLayer0, BitmapUnit) * ((1ull << layer) - 1);
    }

    inline bool Get(int layer, uint64_t block) {
        uint64_t addr = IndexOfLayer(layer) + block / BitmapUnit;
        uint64_t offset = block % BitmapUnit;
   
        return (m_Bitmap[addr] & (1 << offset)) != 0;
    }

    inline void Set(int layer, uint64_t block, bool value) {
        uint64_t addr = IndexOfLayer(layer) + block / BitmapUnit;
        uint64_t offset = block % BitmapUnit;
    }

private:
    uint64_t m_SmallBlockSize;
    uint64_t m_BigBlockSize;
    uint8_t* m_Bitmap;
    uint64_t m_BitmapSize;
    uint64_t m_BlocksLayer0;

    static constexpr size_t BitmapUnit = sizeof(m_Bitmap[0]) * 8;
};
