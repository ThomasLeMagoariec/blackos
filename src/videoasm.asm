[bits 32]

global set_13h_mode
set_13h_mode:
    mov ax, 0013h
    int 0x10
    ret

