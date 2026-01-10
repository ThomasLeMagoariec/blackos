#include "buddy_allocator.h"

BuddyAllocator* buddy_allocator_init(RegionBlocks regions[], size_t regionCount) {
    BuddyAllocator* buddy;

    buddy->SmallBlockSize = buddy->BlockSize;
    buddy->BigBlockSize = buddy->BlockSize * BIG_BLOCK_MULT;
    buddy->BlocksLayer0 = math_divRoundUp(buddy->MemSizeBytes, buddy->SmallBlockSize);
    buddy->BitmapUnit = sizeof(buddy->Bitmap[0]*8);

    return buddy;
}


uint64_t BlocksOnLayer(BuddyAllocator* buddy, int layer) {
    return (1ull << layer) * buddy->BlocksLayer0;
}

uint64_t IndexOfLayer(BuddyAllocator* buddy, int layer) {
  return math_divRoundUp(buddy->BlocksLayer0, buddy->BitmapUnit) *
         ((1ull << layer) - 1);
}


int Get(BuddyAllocator* buddy, int layer, uint64_t block) {
    uint64_t addr = IndexOfLayer(buddy, layer) + block / buddy->BitmapUnit;
    uint64_t offset = block % buddy->BitmapUnit;

    return (buddy->Bitmap[addr] & (1 << offset)) != 0;
}

int Set(BuddyAllocator* buddy, int layer, uint64_t block, int value) {
    uint64_t addr = IndexOfLayer(buddy, layer) + block / buddy->BitmapUnit;
    uint64_t offset = block % buddy->BitmapUnit;

    if (value)  buddy->Bitmap[addr] |=  (1 << offset);
    else        buddy->Bitmap[addr] &= ~(1 << offset);
    return 0;
    
}
