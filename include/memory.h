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

#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"
#include "screen.h"

#define HEAP_START 0x100000
#define HEAP_SIZE 0x100000
#define BLOCK_SIZE 16

typedef struct memory_block {
    uint32 size;
    int is_free;
    struct memory_block* next;
} memory_block_t;

void init_memory();
void* kmalloc(uint32 size);
void kfree(void* ptr);
void print_memory_stats();
uint32 get_free_memory();
uint32 get_used_memory();

#endif
