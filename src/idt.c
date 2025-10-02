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

#include "../include/idt.h"
#include "../include/util.h"

idt_entry_t idt[IDT_ENTRIES];
idt_ptr_t idt_reg;

void set_idt_gate(int n, uint32 handler) {
    idt[n].low_offset = handler & 0xFFFF;
    idt[n].sel = KERNEL_CS;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E;
    idt[n].high_offset = (handler >> 16) & 0xFFFF;
}

void set_idt() {
    idt_reg.limit = (sizeof(idt_entry_t) * IDT_ENTRIES) - 1;
    idt_reg.base = (uint32)&idt;

    memory_set((uint8*)&idt, 0, sizeof(idt_entry_t) * IDT_ENTRIES);
    __asm__ __volatile__("lidtl (%0)" : : "r" (&idt_reg));
}