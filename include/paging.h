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

#ifndef PAGING_H
#define PAGING_H

#include "types.h"

#define PAGE_SIZE 4096
#define PAGE_DIRECTORY_SIZE 1024
#define PAGE_TABLE_SIZE 1024
#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR 1024

#define PAGE_PRESENT 0x1
#define PAGE_WRITE 0x2
#define PAGE_USER 0x4
#define PAGE_WRITETHROUGH 0x8
#define PAGE_CACHE_DISABLE 0x10
#define PAGE_ACCESSED 0x20
#define PAGE_DIRTY 0x40
#define PAGE_SIZE_4MB 0x80
#define PAGE_GLOBAL 0x100

#define PAGE_FRAME 0xFFFFF000

typedef uint32 page_t;
typedef uint32 page_table_t[PAGE_TABLE_SIZE];
typedef uint32 page_directory_t[PAGE_DIRECTORY_SIZE];

typedef struct page_directory_entry {
    uint32 present : 1;
    uint32 read_write : 1;
    uint32 user_supervisor : 1;
    uint32 write_through : 1;
    uint32 cache_disable : 1;
    uint32 accessed : 1;
    uint32 reserved : 1;
    uint32 page_size : 1;
    uint32 global : 1;
    uint32 available : 3;
    uint32 frame : 20;
} __attribute__((packed)) page_directory_entry_t;

typedef struct page_table_entry {
    uint32 present : 1;
    uint32 read_write : 1;
    uint32 user_supervisor : 1;
    uint32 write_through : 1;
    uint32 cache_disable : 1;
    uint32 accessed : 1;
    uint32 dirty : 1;
    uint32 reserved : 1;
    uint32 global : 1;
    uint32 available : 3;
    uint32 frame : 20;
} __attribute__((packed)) page_table_entry_t;

typedef struct page_directory_struct {
    page_directory_t directory;
    page_table_t* tables[PAGES_PER_DIR];
    uint32 physical_tables[PAGES_PER_DIR];
    uint32 physical_addr;
} page_directory_struct_t;

void init_paging();
void enable_paging();
void switch_page_directory(page_directory_struct_t* dir);
page_directory_struct_t* get_kernel_directory();
page_directory_struct_t* get_current_directory();
page_directory_struct_t* clone_directory(page_directory_struct_t* src);

void map_page(uint32 virtual_addr, uint32 physical_addr, uint32 flags);
void unmap_page(uint32 virtual_addr);
uint32 get_physical_address(uint32 virtual_addr);
void* get_page(uint32 virtual_addr, int make, page_directory_struct_t* dir);

void page_fault_handler(uint32 error_code);
void flush_tlb_entry(uint32 virtual_addr);
void flush_tlb();

void* valloc(uint32 size);
void vfree(void* ptr);
void map_kernel_space();
void identity_map(uint32 start, uint32 end);

#endif
