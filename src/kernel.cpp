#include "kernel.hpp"
#include "irq.h"
#include "memory.hpp"

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
    HAL_Initialize();
    mem_ctx ctx = mem_Initialize(mbi);

    int *ptr = (int *)malloc(20);

    for (int i = 0; i < 20; ++i)
        ptr[i] = i;
    printf("first element: %d\n", ptr[0]);

    // exercise new-style realloc (no old size parameter)
    ptr = (int*)realloc(ptr, 40);
    if (ptr) {
        for (int i = 20; i < 40; ++i)
            ptr[i] = i;
        printf("after grow element 30: %d\n", ptr[30]);
    }

    free(ptr);


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

