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
#define MEM_BLOCK_SIZE 16
#define HEAP_MAGIC 0xDEADBEEF
#define MIN_ALLOC_SIZE 16
#define ALIGNMENT 16

typedef enum {
    ALLOC_FIRST_FIT,
    ALLOC_BEST_FIT,
    ALLOC_WORST_FIT
} alloc_strategy_t;

typedef struct memory_block {
    uint32 magic;
    uint32 size;
    int is_free;
    struct memory_block* next;
    struct memory_block* prev;
} memory_block_t;

typedef struct heap_stats {
    uint32 total_size;
    uint32 used_size;
    uint32 free_size;
    uint32 num_blocks;
    uint32 num_allocs;
    uint32 num_frees;
    uint32 num_coalesces;
} heap_stats_t;

void init_memory();
void* kmalloc(size_t size);
void* kmalloc_a(size_t size, size_t alignment);
void* kcalloc(size_t num, size_t size);
void* krealloc(void* ptr, size_t size);
void kfree(void* ptr);

void set_alloc_strategy(alloc_strategy_t strategy);
void print_memory_stats();
void print_heap_blocks();
uint32 get_free_memory();
uint32 get_used_memory();
heap_stats_t* get_heap_stats();

void* kmalloc_p(size_t size, uint32* phys);
void* kmalloc_ap(size_t size, size_t alignment, uint32* phys);

#endif
