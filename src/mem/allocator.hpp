#ifndef ALLOCATOR
#define ALLOCATOR

#include <stdint.h>

typedef enum{
    FREE,
    RESERVED,
    UNMAPED,
    ALLOCATOR
} RegionType;

typedef struct {
    void* Base;
    uint64_t Size;
    RegionType Type;
} Region;

typedef struct {
    uint64_t Base;
    uint64_t Size;
    RegionType Type;
} RegionBlocks;

#endif /* ALLOCATOR */
