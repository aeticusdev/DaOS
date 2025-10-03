/*
 * DaOS - Simple Operating System
 * Copyright (C) 2025 Mostafizur Rahman
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "../include/irq.h"
#include "../include/idt.h"

static irq_handler_t irq_handlers[16] = {0};

void irq_remap() {
    outportb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outportb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    
    outportb(PIC1_DATA, 0x20);
    outportb(PIC2_DATA, 0x28);
    
    outportb(PIC1_DATA, 4);
    outportb(PIC2_DATA, 2);
    
    outportb(PIC1_DATA, ICW4_8086);
    outportb(PIC2_DATA, ICW4_8086);
    
    outportb(PIC1_DATA, 0x0);
    outportb(PIC2_DATA, 0x0);
}

void irq_install() {
    irq_remap();
    
    set_idt_gate(32, (uint64)irq0);
    set_idt_gate(33, (uint64)irq1);
    set_idt_gate(34, (uint64)irq2);
    set_idt_gate(35, (uint64)irq3);
    set_idt_gate(36, (uint64)irq4);
    set_idt_gate(37, (uint64)irq5);
    set_idt_gate(38, (uint64)irq6);
    set_idt_gate(39, (uint64)irq7);
    set_idt_gate(40, (uint64)irq8);
    set_idt_gate(41, (uint64)irq9);
    set_idt_gate(42, (uint64)irq10);
    set_idt_gate(43, (uint64)irq11);
    set_idt_gate(44, (uint64)irq12);
    set_idt_gate(45, (uint64)irq13);
    set_idt_gate(46, (uint64)irq14);
    set_idt_gate(47, (uint64)irq15);
}

void irq_handler(int irq) {
    if(irq_handlers[irq]) {
        irq_handlers[irq]();
    }
    
    if(irq >= 8) {
        outportb(PIC2_COMMAND, PIC_EOI);
    }
    outportb(PIC1_COMMAND, PIC_EOI);
}

void irq_set_handler(int irq, irq_handler_t handler) {
    if(irq >= 0 && irq < 16) {
        irq_handlers[irq] = handler;
    }
}

void irq_clear_handler(int irq) {
    if(irq >= 0 && irq < 16) {
        irq_handlers[irq] = 0;
    }
}
