#include "gdt.h"
#include "dbg_stdio.h"
#include "io.h"


GDTEntry g_GDT[] = {
    GDT_ENTRY(0, 0, 0, 0), // null descriptor
    GDT_ENTRY(  0,
                0xFFFFF,
                GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_CODE_SEGMENT | GDT_ACCESS_CODE_READABLE,
                GDT_FLAG_32BIT | GDT_FLAG_GRANNULARITY_4K
    ),
    GDT_ENTRY(  0,
                0xFFFFF,
                GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 | GDT_ACCESS_DATA_SEGMENT | GDT_ACCESS_DATA_WRITABLE,
                GDT_FLAG_32BIT | GDT_FLAG_GRANNULARITY_4K
    )

};

GDTDescriptor g_GDTDescriptor = { sizeof(g_GDT) - 1, g_GDT };



void i686_GDT_Initialize() {
    dbg_printf("Initialize GDT...\t");
    i686_GDT_Load(&g_GDTDescriptor, i686_GDT_CODE_SEGMENT, i686_GDT_DATA_SEGMENT);
    dbg_printf("ok\n");
}
