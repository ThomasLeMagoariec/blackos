org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A

;
; FAT12 Headers
;
jmp short start
nop

bdb_oem:                    db "MSWIN4.1"               ; 8 bytes
bdb_bytes_per_sector:       dw 512
bdb_sectors_per_clusrer:    db 1
bdb_reserved_sectors:       dw 1
bdb_fat_count:              db 2
bdb_dir_entries_count:      dw 0xE0
bdb_total_sectors:          dw 2880                     ; 2880 * 512 = 1440k
bdb_media_descriptor_type:  db 0xF0                     ; 0xF0 = 3.5" floppy disk
bdb_sectors_per_fat:        dw 9
bdb_sectors_per_track:      dw 18
bdb_heads:                  dw 2
bdb_hidden_sectors:         dd 0
bdb_large_sector_count:     dd 0

; extended boot record

ebr_drive_number:           db 0                        ; 0x00 = Floppy Disk, 0x80 = HDD
                            db 0                        ; reserved
ebr_signature:              db 0x29
ebr_volume_id:              db 0x12, 0x34, 0x56, 0x78   ; serial number
ebr_volume_label:           db "BLACK      "            ; 11 bytes, padded with spaces
ebr_system_id:              db "FAT12   "               ; 08 bytes, padded with spaces


;
; Code
;

start:
    jmp main

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
    lodsb                                               ; loads next char in al
    or al, al                                           ; check if next char is null
    jz .done

    mov ah, 0x0e
    mov bh, 0
    int 0x10                                            ; bios interrupt

    jmp .loop

.done:
    pop ax
    pop si
    ret


main:
    ; setpu data segments
    mov ax, 0                                           ; can't write to ds/es direclty
    mov ds, ax
    mov es, ax

    ; setup stack
    mov ss, ax
    mov sp, 0x7C00                                      ; stack grows downwards

    ; read from disk
    ; BIOS should set dl to drive number
    mov [ebr_drive_number], dl
    mov ax, 1                                           ; LBA = 1, second sector from disk
    mov cl, 1                                           ; 1 sector to read
    mov bx, 0x7E00                                      ; data should be after the bootloader
    call disk_read

    ; print message
    mov si, msg_hello
    call puts

    cli
    hlt


;
; Error Handlers
;

floppy_error:
    mov si, msg_read_failed
    call puts
    jmp wait_key_and_reboot

wait_key_and_reboot:
    mov ah, 0
    int 0x16                                            ; wait for key press
    jmp 0FFFFh:0                                        ; jump to start of bios, should reboot

.halt:
    cli                                                 ; disable interrupts, avoid getting out of halt state
    hlt


;
; Disk Routines
;

;
; Converts and LBA address to a CHS address
; Arguments:
;   - ax: LBA address
; Returns:
;   - cx[bits 0-5]: sector number
;   - dh: head
;

lba_to_chs:
    push ax
    push dx

    xor dx, dx                                          ; dx = 0
    div word [bdb_sectors_per_track]                    ; ax = LBA / sectors_per_track
                                                        ; dx = LBA % sectors_per_track
    
    inc dx                                              ; dx = (LBA % sectors_per_track) + 1 = sector
    mov cx, dx                                          ; cx = sector

    xor dx, dx                                          ; dx = 0
    div word [bdb_heads]                                ; ax = (LBA / sectors_per_track) / heads = cylinder
                                                        ; dx = (LBA / sectors_per_track) % heads = head

    mov dh, dl                                          ; dh = head
    mov ch, al                                          ; ch = cylinder (lower 8 bits)
    shl al, 6
    or cl, ah                                           ; put upper 2 bits of cylinder in CL

    pop ax
    mov dl, al                                          ; restore dl
    pop ax                                              ; not sure why pop a second time ???
    ret


;
; Read sectors from disk
; Arguments:
;   - ax: LBA address
;   - cl: number of sectors to read (up to 128)
;   - dl: drive number
;   - es:bx: memory address to store read data
;
disk_read:
    ; save registers that'll be modified
    push ax
    push bx
    push cx
    push dx
    push di

    push cx                                             ; temporarily save cl: numbers of sectors to read
    call lba_to_chs                                     ; get chs
    pop ax                                              ; al = number of sectors to read

    mov ah, 0x02
    mov di, 3                                           ; retry count

.retry:
    pusha                                               ; save all registers, don't known what the BIOS will modify
    stc                                                 ; not every bios does it
    int 0x13                                            ; jump if carry not set
    jnc .done

    ; read failed
    popa
    call disk_reset

    dec di
    test di, di
    jnz .retry

.fail:
    ; once no attempts are left
    jmp floppy_error

.done:
    popa

    ; resotre modified registers
    pop di
    pop dx
    pop cx
    pop bx
    pop ax

    ret

;
; Resets disk controller
; Arguments:
;   - dl: drive number
;
disk_reset:
    pusha

    mov ah, 0
    stc
    int 0x13
    jc floppy_error

    popa
    ret

msg_hello:              db "Hello, World!", ENDL, 0
msg_read_failed:        db "[OS] Read from disk failed!", ENDL, 0

times 510-($-$$) db 0
dw 0xAA55