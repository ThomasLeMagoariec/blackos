#include "stdint.h"
#include "stdio.h"

void _cdecl cstart_(uint16_t bootDrive)
{
    puts("Hello, World from C!\r\n");
    printf("Formatted %% %c %s\r\n", 'a', "abc");
    printf("Formatted %d\r\n", 12);
    for (;;);
}
