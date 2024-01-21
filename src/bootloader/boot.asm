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
    ; setpu data segments
    mov ax, 0                                           ; can't write to ds/es direclty
    mov ds, ax
    mov es, ax

    ; setup stack
    mov ss, ax
    mov sp, 0x7C00                                      ; stack grows downwards

    ; some BIOSes start at 07C0:0000 instead if 0000:07C0
    push es
    push word .after
    retf

.after:
    ; read from disk
    ; BIOS should set dl to drive number
    mov [ebr_drive_number], dl

    ; show loading msg
    mov si, msg_loading
    call puts

    ; read drive parameters (sectors per track & head count)
    push es
    mov ah, 0x8
    int 0x13
    jc floppy_error
    pop es

    and cl, 0x3F                                        ; remove top 2 bits
    xor ch, ch
    mov [bdb_sectors_per_track], cx                     ; sectors count

    inc dh
    mov [bdb_heads], dh                                 ; head count

    ; read FAT root directory
    mov ax, [bdb_sectors_per_fat]                       ; LBA of root dir = reserved + fats * sectors_per_fat
    mov bl, [bdb_fat_count]
    xor bh, bh
    mul bx                                              ; ax = (fats * sectors_per_fat)
    add ax, [bdb_reserved_sectors]                      ; ax = LBA of root directory
    push ax
    
    ; compute size of root dir = 32 * number_of_entries / bytes_per_sector
    mov ax, [bdb_sectors_per_fat]
    shl ax, 5                                           ; ax *= 32
    xor dx, dx                                          ; dx = 0
    div word [bdb_bytes_per_sector]                     ; nmuber of sectors we need to read

    test dx, dx                                         ; if dx != 0 add 1
    jz .root_dir_after
    inc ax                                              ; division remainder != 0 add 1
                                                        ; this only partially full sector
        
.root_dir_after:
    ; read root dir
    mov cl, al                                          ; cl = number of sectors to read = size of root dir
    pop ax                                              ; ax = LBA of root dir
    mov dl, [ebr_drive_number]                          ; dl = drive number
    mov bx, buffer                                      ; es:bx = buffer
    call disk_read

    ; search for kernel.bin
    xor bx, bx
    mov di, buffer

.search_kernel:
    mov si, file_kernel_bin
    mov cx, 11                                          ; compare up to 11 chars
    push di

    repe cmpsb
    pop di

    je .found_kernel

    add di, 32
    inc bx
    cmp bx, [bdb_dir_entries_count]
    jl .search_kernel

    ; kernel not found

    jmp kernel_not_found_error

.found_kernel:
    ; di should have the address of the entry

    mov ax, [di + 26]                                   ; first logical cluster field (offset 26)
    mov [kernel_cluster], ax

    ; load FAT from disk into memory
    mov ax, [bdb_reserved_sectors]
    mov bx, buffer
    mov cl, [bdb_sectors_per_fat]
    mov dl, [ebr_drive_number]

    call disk_read

    ; read kernel and process FAT chain

    mov bx, KERNEL_LOAD_SEGMENT
    mov es, bx
    mov bx, KERNEL_LOAD_OFFSET

.load_kernel_loop:
    ; Read next cluster
    mov ax, [kernel_cluster]
    add ax, 31                                          ; first cluster = (cluster number - 2) * sectors_per_cluster + start_sector
                                                        ; start sector = reserved + fats + root directory size = 1 + 18 + 134 = 33 (153)
    
    mov cl, 1
    mov dl, [ebr_drive_number]
    call disk_read

    add bx, [bdb_bytes_per_sector]

    ; compute location of next cluster
    mov ax, [kernel_cluster]
    mov cx, 3
    mul cx
    mov cx, 2
    div cx                                              ; ax = index of entry in FAT, dx = cluster mod 2

    mov si, buffer
    add si, ax
    mov ax, [ds:si]                                     ; read entry from FAT table at index ax

    or dx, dx
    jz .even

.odd:
    shr ax, 4
    jmp .next_cluster_after

.even:
    and ax, 0x0FFF

.next_cluster_after:
    cmp ax, 0x0FF8                                      ; end of chain
    jae .read_finish

    mov [kernel_cluster], ax
    jmp .load_kernel_loop

.read_finish:
    ; jump to our kernel
    mov dl, [ebr_drive_number]                          ; boot device in dl

    mov ax, KERNEL_LOAD_SEGMENT                         ; set segment registers
    mov ds, ax
    mov es, ax

    jmp KERNEL_LOAD_SEGMENT:KERNEL_LOAD_OFFSET

    jmp wait_key_and_reboot                             ; should never happen


    cli
    hlt


;
; Error Handlers
;

floppy_error:
    mov si, msg_read_failed
    call puts
    jmp wait_key_and_reboot

kernel_not_found_error:
    mov si, msg_kernel_not_found
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

msg_loading:            db "Loading...", ENDL, 0
msg_read_failed:        db "Read from disk failed!", ENDL, 0
msg_kernel_not_found:   db "Couldn't find kernel", ENDL, 0
file_kernel_bin         db "KERNEL  BIN"
kernel_cluster          dw 0

KERNEL_LOAD_SEGMENT     equ 0x2000
KERNEL_LOAD_OFFSET      equ 0

times 510-($-$$) db 0
dw 0xAA55

buffer: