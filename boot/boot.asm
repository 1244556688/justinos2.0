[org 0x7c00]
KERNEL_OFFSET equ 0x1000

    ; BIOS stores boot drive in DL
    mov [BOOT_DRIVE], dl

    ; Setup stack
    mov bp, 0x9000
    mov sp, bp

    mov si, MSG_BOOT
    call print_string

    call load_kernel
    call switch_to_pm

    jmp $ ; never reached

; 16-bit real mode print
print_string:
    pusha
.loop:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0e
    int 0x10
    jmp .loop
.done:
    popa
    ret

; Load kernel from disk
load_kernel:
    mov si, MSG_LOAD
    call print_string

    mov bx, KERNEL_OFFSET
    mov ah, 0x02
    mov al, 31       ; Load 31 sectors
    mov ch, 0x00     ; Cylinder 0
    mov dh, 0x00     ; Head 0
    mov cl, 0x02     ; Sector 2
    int 0x13
    jc disk_error
    ret

disk_error:
    mov si, MSG_ERROR
    call print_string
    jmp $

; Switch to Protected Mode
switch_to_pm:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    jmp CODE_SEG:init_pm

; Global Descriptor Table
gdt_start:
    dd 0x0
    dd 0x0
gdt_code:
    dw 0xffff     ; Limit
    dw 0x0        ; Base
    db 0x0        ; Base
    db 10011010b  ; Flags
    db 11001111b  ; Flags
    db 0x0        ; Base
gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

[bits 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ebp, 0x90000
    mov esp, ebp

    call KERNEL_OFFSET ; Jump to Kernel
    jmp $

BOOT_DRIVE db 0
MSG_BOOT db "Booting justinOS...", 13, 10, 0
MSG_LOAD db "Loading kernel into memory...", 13, 10, 0
MSG_ERROR db "Disk read error!", 13, 10, 0

; Bootsector padding
times 510-($-$$) db 0
dw 0xaa55
