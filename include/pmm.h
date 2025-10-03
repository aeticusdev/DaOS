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

#ifndef PMM_H
#define PMM_H

#include "types.h"

#define PMM_BLOCK_SIZE 4096
#define PMM_BLOCKS_PER_BYTE 8
#define MAX_MEMORY_SIZE 0x10000000
#define MEMORY_MAP_SIZE (MAX_MEMORY_SIZE / PMM_BLOCK_SIZE / PMM_BLOCKS_PER_BYTE)

typedef struct multiboot_memory_map {
    uint32 size;
    uint32 base_addr_low;
    uint32 base_addr_high;
    uint32 length_low;
    uint32 length_high;
    uint32 type;
} __attribute__((packed)) multiboot_memory_map_t;

typedef struct multiboot_info {
    uint32 flags;
    uint32 mem_lower;
    uint32 mem_upper;
    uint32 boot_device;
    uint32 cmdline;
    uint32 mods_count;
    uint32 mods_addr;
    uint32 syms[4];
    uint32 mmap_length;
    uint32 mmap_addr;
} __attribute__((packed)) multiboot_info_t;

void init_pmm(uint32 total_memory);
void pmm_init_region(uint32 base, uint32 size);
void pmm_deinit_region(uint32 base, uint32 size);

uint32 pmm_allocate_page();
void pmm_free_page(uint32 page);
uint32 pmm_allocate_pages(uint32 count);
void pmm_free_pages(uint32 page, uint32 count);

uint32 pmm_get_total_memory();
uint32 pmm_get_free_memory();
uint32 pmm_get_used_memory();
uint32 pmm_get_block_count();

int pmm_is_page_allocated(uint32 page);
void pmm_print_stats();

uint32 pmm_allocate_dma_pages(uint32 count);

#endif
