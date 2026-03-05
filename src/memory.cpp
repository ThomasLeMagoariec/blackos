#include "memory.hpp"
#include "memdetect.h"

BootParams* g_BootParams;
LinkedListAllocator* alloc;

LinkedListAllocator* get_alloc() {
    return alloc;
}

ptr_t malloc(uint32_t blocks) {
    return alloc->Allocate(blocks);
}

ptr_t realloc(ptr_t base, uint32_t blocks) {
    return alloc->Reallocate(base, blocks);
}

void free(ptr_t base, uint32_t /*blocks*/) {
    alloc->Free(base);
}

mem_ctx mem_Initialize(struct multiboot_info* mbi) {
    mem_ctx ctx;
    init_memory_info(mbi, g_BootParams);

    MemoryRegion* regions = memdetect_getMemRegions();
    int region_count = count_memory_regions(mbi);


    Region new_regions[128];
    if (region_count > 128) {
        printf("Error: Exceeded maximum number of memory regions (128).\n");
        while(1);
    }

    for (int i = 0; i < region_count; i++) {
        new_regions[i].Base = (void*)regions[i].Begin;
        new_regions[i].Size = regions[i].Length;
        new_regions[i].Type = (RegionType)(regions[i].Type-1);

        printf("region: %d, base: %p, size: 0x%llx, type: %d\n",
            i,
            new_regions[i].Base,
            new_regions[i].Size,
            new_regions[i].Type
        );
    }

    dbg_printf("REGION COUNT: %d\n", region_count);

    bool res = ctx.g_Allocator.Initialize(4096, new_regions, region_count);
    alloc = &ctx.g_Allocator;

    if (!res) {
        printf("Failed to init allocator\n");
    } else {
        printf("Alloc good\n");
    }

    return ctx;
}
