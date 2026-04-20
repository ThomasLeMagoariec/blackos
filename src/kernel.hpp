#ifndef KERNEL_H
#define KERNEL_H


#include "memdetect.h"
#include "hal.h"
#include "vga.h"
#include "stdio.h"
#include "dbg_stdio.h"
#include "keyboard.h"
#include "mem/buddy.hpp"
#include "memory.hpp"
#include "game/game.h"

#ifdef __cplusplus
    extern "C" {
#endif

void kernel_main(uint32_t magic, struct multiboot_info* mbi);

#ifdef __cplusplus
    }
#endif


#endif
