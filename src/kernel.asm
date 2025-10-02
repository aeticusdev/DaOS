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

section .text
align 4
dd      0x1BADB002
dd      0x00
dd      - (0x1BADB002+0x00)

global start
extern kmain

start:
    cli
    call kmain

    call Shutdown
    hlt

Shutdown:
    .fnstart
    mov ax, 0x1000
    mov ax, ss
    mov sp, 0xf000
    mov ax, 0x5307
    mov bx, 0x0001
    int 0x15
    .fnend

WaitForEnter:
    .fnstart
    mov ah, 0
    int 0x16
    cmp al, 0x0D
    jne WaitForEnter
    .fnend
    ret
