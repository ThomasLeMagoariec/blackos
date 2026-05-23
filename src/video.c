#include "video.h"
#include "memdetect.h"
#include "dbg_stdio.h"

Video g_video;


void video_init(struct multiboot_info* mbi) {
    g_video.addr = (volatile char*)0xA0000;
    //g_video.mbi = mbi;
    dbg_printf("framebuffer_type: %x\n", mbi->framebuffer_type);
    dbg_printf("framebuffer_bpp: %x\n", mbi->framebuffer_bpp);
    dbg_printf("framebuffer_pitch: %x\n", mbi->framebuffer_pitch);
    dbg_printf("framebuffer_width: %x\n", mbi->framebuffer_width);
    dbg_printf("framebuffer_height: %x\n", mbi->framebuffer_height);
}

void video_putpixel(int x, int y, int color) {
    g_video.addr[y * 320 + x] = color;
}

void fill_screen(struct multiboot_info* mbi, uint32_t color) {
    volatile uint8_t* fb =
        (volatile uint8_t*)(uintptr_t)mbi->framebuffer_addr;

    uint32_t bytes_per_pixel =
        mbi->framebuffer_bpp / 8;

    for (uint32_t y = 0; y < mbi->framebuffer_height; y++) {
        for (uint32_t x = 0; x < mbi->framebuffer_width; x++) {

            uint32_t pixel =
                y * mbi->framebuffer_pitch +
                x * bytes_per_pixel;

            fb[pixel + 0] = 0x00;
            fb[pixel + 1] = 0x00;
            fb[pixel + 2] = 0xFF;
        }
    }
}
