BITS 32
SECTION .multiboot

ALIGN 4
    MULTIBOOT_MAGIC     equ 0x1BADB002
    MULTIBOOT_FLAGS     equ 0x1000003
    MULTIBOOT_CHECKSUM  equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM

SECTION .text
global start
extern kernel_main

start:
    cli
    push ebx        ; multiboot_info*
    push eax        ; multiboot magic
    call kernel_main
    hlt
