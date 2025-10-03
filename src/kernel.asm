; DaOS - Simple Operating System
; Copyright (C) 2025 Mostafizur Rahman
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <https://www.gnu.org/licenses/>.

bits 32

section .multiboot
align 8
multiboot_header_start:
    dd 0xE85250D6                ; Multiboot2 magic number
    dd 0                         ; Architecture: 0 (i386)
    dd multiboot_header_end - multiboot_header_start  ; Header length
    dd -(0xE85250D6 + 0 + (multiboot_header_end - multiboot_header_start))  ; Checksum
    
    ; End tag
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
multiboot_header_end:

section .bss
align 4096
p4_table:
    resb 4096
p3_table:
    resb 4096
p2_table:
    resb 4096
stack_bottom:
    resb 16384
stack_top:

section .text
global start
extern kmain

start:
    mov esp, stack_top
    
    call check_multiboot
    call check_cpuid
    call check_long_mode
    
    call setup_page_tables
    call enable_paging
    
    lgdt [gdt64.pointer]
    jmp gdt64.code:long_mode_start

check_multiboot:
    cmp eax, 0x36d76289
    jne .no_multiboot
    ret
.no_multiboot:
    mov al, "0"
    jmp error

check_cpuid:
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    cmp eax, ecx
    je .no_cpuid
    ret
.no_cpuid:
    mov al, "1"
    jmp error

check_long_mode:
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode
    ret
.no_long_mode:
    mov al, "2"
    jmp error

setup_page_tables:
    mov eax, p3_table
    or eax, 0b11
    mov [p4_table], eax
    
    mov eax, p2_table
    or eax, 0b11
    mov [p3_table], eax
    
    mov ecx, 0
.map_p2_table:
    mov eax, 0x200000
    mul ecx
    or eax, 0b10000011
    mov [p2_table + ecx * 8], eax
    inc ecx
    cmp ecx, 512
    jne .map_p2_table
    
    ret

enable_paging:
    mov eax, p4_table
    mov cr3, eax
    
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    
    ret

error:
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte  [0xb800a], al
    hlt

bits 64
long_mode_start:
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    call kmain
    
    cli

    hlt

section .rodata
gdt64:
    dq 0
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53)
.pointer:
    dw $ - gdt64 - 1
    dq gdt64
