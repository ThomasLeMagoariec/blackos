#ifndef VIDEO_H
#define VIDEO_H

#include <stdint.h>
#ifdef __cplusplus
    extern "C" {
#endif

typedef struct {
    volatile char *addr;
    struct multiboot_info *mbi;
} Video;

void video_init(struct multiboot_info* mbi);
void video_putpixel(int x, int y, int color);
void fill_screen(struct multiboot_info* mbi, uint32_t color);

void __attribute__((cdecl)) set_13h_mode();

#ifdef __cplusplus
    }
#endif

#endif /* VIDEO_H */
