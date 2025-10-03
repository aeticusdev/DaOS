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

global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

extern irq_handler

; Macro for IRQ handlers in x86_64
%macro IRQ_HANDLER 1
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    mov rdi, %1           ; First argument (irq number) in rdi
    call irq_handler
    
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    iretq
%endmacro

irq0:
    IRQ_HANDLER 0

irq1:
    IRQ_HANDLER 1

irq2:
    IRQ_HANDLER 2

irq3:
    IRQ_HANDLER 3

irq4:
    IRQ_HANDLER 4

irq5:
    IRQ_HANDLER 5

irq6:
    IRQ_HANDLER 6

irq7:
    IRQ_HANDLER 7

irq8:
    IRQ_HANDLER 8

irq9:
    IRQ_HANDLER 9

irq10:
    IRQ_HANDLER 10

irq11:
    IRQ_HANDLER 11

irq12:
    IRQ_HANDLER 12

irq13:
    IRQ_HANDLER 13

irq14:
    IRQ_HANDLER 14

irq15:
    IRQ_HANDLER 15
