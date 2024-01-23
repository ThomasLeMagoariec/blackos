bits 16

section _TEXT class=CODE

;
; int 0x10 ah=0Eh
; args: character, page
;
global _x86_Video_WriteCharTeletype
_x86_Video_WriteCharTeletype:
    ; make new call frame
    push bp                                                     ; save old call frame
    mov bp, sp                                                  ; initialize new call frame

    push bx                                                     ; save bx

    ; [bp + 0] - old frame
    ; [bp + 2] - return address (small memory model => 2 bytes)
    ; [bp + 4] - first arg (character)
    ; [bp + 6] - second arg (page)
    ; note: bytes are converted to words; single bytes can't be pushed
    mov ah, 0Eh
    mov al, [bp + 4]
    mov bh, [bp + 6]

    int 0x10

    pop bx                                                      ; restore bx

    ; restore old call frame
    mov sp, bp
    pop bp
    ret