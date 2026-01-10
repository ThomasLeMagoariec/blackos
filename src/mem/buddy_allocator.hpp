#ifndef BUDDY_ALLOCATOR
#define BUDDY_ALLOCATOR

#include "../math/math.h"
#include "allocator.h"

#define LAYER_COUNT 10

#define BIG_BLOCK_MULT (1ull << (LAYER_COUNT - 1))

typedef struct {
    uint64_t SmallBlockSize;
    uint64_t BigBlockSize;
    uint64_t BlockSize;
    uint8_t* Bitmap;
    uint64_t BitmapSize;
    uint64_t BlocksLayer0;
    size_t BitmapUnit;
    uint64_t MemSizeBytes;
    uint64_t MemeSize;
} BuddyAllocator;

BuddyAllocator* buddy_allocator_init(RegionBlocks regions[], size_t regionCount);

// memory
void* allocate(uint32_t blocks);
void free(void* base, uint32_t blocks);

// helper functions
uint64_t BlocksOnLayer(BuddyAllocator* buddy, int layer);
uint64_t IndexOfLayer(BuddyAllocator* buddy, int layer);
int Get(BuddyAllocator* buddy, int layer, uint64_t block);
int Set(BuddyAllocator* buddy, int layer, uint64_t block, int value);


#endif /* BUDDY_ALLOCATOR */
