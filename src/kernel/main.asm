org 0x0
bits 16

%define ENDL 0x0D, 0x0A

start:


    ; print message
    mov si, msg_hello
    call puts

.halt:
    cli
    hlt

;
; Prints a string to the screen
; Arguments:
;   - ds:si points to string
;
puts:
    ; save registers we will modify
    push si
    push ax

.loop:
    lodsb                      ; loads next char in al
    or al, al                  ; check if next char is null
    jz .done

    mov ah, 0x0e
    mov bh, 0
    int 0x10                    ; bios interrupt

    jmp .loop

.done:
    pop ax
    pop si
    ret


msg_hello: db "Hello, World from kernel!", ENDL, 0