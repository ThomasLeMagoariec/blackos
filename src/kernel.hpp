#ifndef KERNEL_H
#define KERNEL_H

#include "memdetect.h"
#include "vga.h"
#include "hal.h"
#include "stdio.h"
#include "dbg_stdio.h"
#include "keyboard.h"
#include "mem/allocator.hpp"

#ifdef __cplusplus
    extern "C" {
#endif

void kernel_main(uint32_t magic, struct multiboot_info* mbi);

#ifdef __cplusplus
    }
#endif


#endif
