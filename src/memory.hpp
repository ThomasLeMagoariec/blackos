#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "mem/allocator.hpp"
#include "mem/LinkedList.hpp"

typedef struct {
    LinkedListAllocator g_Allocator;
} mem_ctx;

#ifdef __cplusplus
extern "C" {
#endif

mem_ctx mem_Initialize(struct multiboot_info* mbi);
LinkedListAllocator* get_alloc();

ptr_t malloc(uint32_t blocks);
ptr_t realloc(ptr_t base, uint32_t newBlocks);
void free(void* base);

#ifdef __cplusplus
}
#endif
