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

#ifndef IDT_H
#define IDT_H

#include "types.h"

#define KERNEL_CS 0x08

#ifdef __x86_64__
typedef struct {
    uint16 low_offset;
    uint16 sel;
    uint8 ist;
    uint8 flags;
    uint16 mid_offset;
    uint32 high_offset;
    uint32 reserved;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16 limit;
    uint64 base;
} __attribute__((packed)) idt_ptr_t;

void set_idt_gate(int n, uint64 handler);
#else
typedef struct {
    uint16 low_offset;
    uint16 sel;
    uint8 always0;
    uint8 flags;
    uint16 high_offset;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16 limit;
    uint32 base;
} __attribute__((packed)) idt_ptr_t;

void set_idt_gate(int n, uint32 handler);
#endif

#define IDT_ENTRIES 256
extern idt_entry_t idt[IDT_ENTRIES];
extern idt_ptr_t idt_reg; 

void set_idt();

#endif