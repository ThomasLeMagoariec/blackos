#include "kernel.hpp"
#include "irq.h"

BootParams* g_BootParams;

void timer(Registers* regs) {

    int scancode = kb_dequeue_key();

    if (scancode != -1) {
        char c = kb_map_scancode(scancode);
        printf("%c", c);
    }


}

void irq_keyboard(Registers* regs) {
    kb_handle_scancode(i686_IRQ_GetBuffer());
}


/*
struct Region {
    void* Base;
    uint64_t Size;
    RegionType Type;
};
*/

/*
typedef struct {
    uint64_t Begin, Length;
    uint32_t Type;
    uint32_t ACPI;
} MemoryRegion;
*/

//static BuddyAllocator kernelAllocator;
//BuddyAllocator g_Allocator;

void kernel_main(uint32_t magic, struct multiboot_info* mbi) {
    init_memory_info(mbi, g_BootParams);

    MemoryRegion* regions = memdetect_getMemRegions();
    int region_count = count_memory_regions(mbi);

    HAL_Initialize();

    Region new_regions[128];
    if (region_count > 128) {
        // Handle error: too many memory regions to fit in our temporary array.
        // For now, we'll just hang or print an error.
        printf("Error: Exceeded maximum number of memory regions (128).\n");
        while(1);
    }

    for (int i = 0; i < region_count; i++) {
        new_regions[i].Base = (void*)regions[i].Begin;
        new_regions[i].Size = regions[i].Length;
        new_regions[i].Type = (RegionType)(regions[i].Type-1);

        printf("region: %d, base: %p, size: 0x%llx, type: %d\n", i, new_regions[i].Base, new_regions[i].Size, new_regions[i].Type);
    }
    /*
    gAllocator = &kernelAllocator;
    gAllocator->Initialize(4096, new_regions, region_count);
    */

    dbg_printf("REGION COUNT: %d\n", region_count);
    BuddyAllocator g_Allocator;
    bool res = g_Allocator.Initialize(4096, new_regions, region_count);

    if (!res) {
        printf("Failed to init allocator\n");
    } else {
        printf("Alloc good");
    }



    /*
    BuddyAllocator* alloc = new BuddyAllocator();
    //BuddyAllocator::Initialize(4096, new_regions, region_count);
    alloc->Initialize(4096, new_regions, region_count);
    */



    i686_IRQ_RegisterHandler(0, timer);
    i686_IRQ_RegisterHandler(1, irq_keyboard);
    

    printf("Hello, World!\n");

    printf("> ");


    while (1); // hang
}

