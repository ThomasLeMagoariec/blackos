#include "memdetech.h"

#define MAX_REGIONS 256

MemoryRegion g_MemoryRegions[MAX_REGIONS];
int g_MemRegionCount;

void print_memmap() {
    for (int i = 0; i < g_MemRegionCount; i++) {
        
        //printf("MEM: base=0x%llx\tlen=0x%llx\ttype=0x%x\n", e->addr, e->len, e->type);
        printf("MEM: base=0x%llx\tlen=0x%llx\ttype=0x%x\n",
                g_MemoryRegions[i].Begin,
                g_MemoryRegions[i].Length,
                g_MemoryRegions[i].Type);
    }
}

int count_memory_regions(struct multiboot_info* mbi) {
    int count = 0;

    for (uint32_t off = 0; off < mbi->mmap_length; ) {
        struct multiboot_mmap_entry* e =
            (struct multiboot_mmap_entry*)(mbi->mmap_addr + off);

        count++;
        off += e->size + sizeof(e->size);
    }

    return count;
}

void init_memory_info(struct multiboot_info* mbi, BootParams* params) {
    if (!(mbi->flags & (1 << 6))) {
        // No memory map — fatal error
        return;
    }

    g_MemRegionCount = count_memory_regions(mbi);


    int i = 0;

    for (uint32_t off = 0; off < mbi->mmap_length; ) {
        struct multiboot_mmap_entry* e =
            (struct multiboot_mmap_entry*)(mbi->mmap_addr + off);

        g_MemoryRegions[i].Begin  = e->addr;
        g_MemoryRegions[i].Length = e->len;
        g_MemoryRegions[i].Type   = e->type;

        // Mark ACPI reclaimable regions
        g_MemoryRegions[i].ACPI = (e->type == 3);

        i++;
        off += e->size + sizeof(e->size);

    }

    params->Memory.RegionCount = g_MemRegionCount;
    params->Memory.Regions     = g_MemoryRegions;

}
