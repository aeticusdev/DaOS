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

#include "../include/paging.h"
#include "../include/memory.h"
#include "../include/screen.h"
#include "../include/string.h"
#include "../include/idt.h"

static page_directory_struct_t* kernel_directory = 0;
static page_directory_struct_t* current_directory = 0;

extern uint32 pmm_allocate_page();
extern void pmm_free_page(uint32 page);

static page_directory_struct_t kernel_dir_struct;
static int paging_initialized = 0;

static page_directory_struct_t* create_page_directory() {
    page_directory_struct_t* dir;
    
    if (!paging_initialized) {
        dir = &kernel_dir_struct;
        paging_initialized = 1;
    } else {
        dir = (page_directory_struct_t*)kmalloc(sizeof(page_directory_struct_t));
        if (!dir) {
            return 0;
        }
    }
    
    memset(dir->directory, 0, sizeof(page_directory_t));
    memset(dir->tables, 0, sizeof(page_table_t*) * PAGES_PER_DIR);
    memset(dir->physical_tables, 0, sizeof(uint32) * PAGES_PER_DIR);
    
    dir->physical_addr = (uintptr)dir->directory;
    
    return dir;
}

void identity_map(uint32 start, uint32 end) {
    uint32 addr;
    for (addr = start; addr < end; addr += PAGE_SIZE) {
        map_page(addr, addr, PAGE_PRESENT | PAGE_WRITE);
    }
}

void map_kernel_space() {
    identity_map(0x0, 0x800000);
}

void map_page(uint32 virtual_addr, uint32 physical_addr, uint32 flags) {
    uint32 page_dir_idx = virtual_addr / (PAGE_SIZE * PAGES_PER_TABLE);
    uint32 page_table_idx = (virtual_addr / PAGE_SIZE) % PAGES_PER_TABLE;
    
    if (!current_directory->tables[page_dir_idx]) {
        uint32 phys = pmm_allocate_page();
        current_directory->tables[page_dir_idx] = (page_table_t*)(uintptr)phys;
        current_directory->physical_tables[page_dir_idx] = phys | PAGE_PRESENT | PAGE_WRITE | (flags & PAGE_USER);
        current_directory->directory[page_dir_idx] = phys | PAGE_PRESENT | PAGE_WRITE | (flags & PAGE_USER);
        
        memset((void*)(uintptr)phys, 0, PAGE_SIZE);
    }
    
    page_table_t* table = current_directory->tables[page_dir_idx];
    (*table)[page_table_idx] = (physical_addr & PAGE_FRAME) | flags;
    
    flush_tlb_entry(virtual_addr);
}

void unmap_page(uint32 virtual_addr) {
    uint32 page_dir_idx = virtual_addr / (PAGE_SIZE * PAGES_PER_TABLE);
    uint32 page_table_idx = (virtual_addr / PAGE_SIZE) % PAGES_PER_TABLE;
    
    if (!current_directory->tables[page_dir_idx]) {
        return;
    }
    
    page_table_t* table = current_directory->tables[page_dir_idx];
    (*table)[page_table_idx] = 0;
    
    flush_tlb_entry(virtual_addr);
}

uint32 get_physical_address(uint32 virtual_addr) {
    uint32 page_dir_idx = virtual_addr / (PAGE_SIZE * PAGES_PER_TABLE);
    uint32 page_table_idx = (virtual_addr / PAGE_SIZE) % PAGES_PER_TABLE;
    
    if (!current_directory->tables[page_dir_idx]) {
        return 0;
    }
    
    page_table_t* table = current_directory->tables[page_dir_idx];
    uint32 page_entry = (*table)[page_table_idx];
    
    if (!(page_entry & PAGE_PRESENT)) {
        return 0;
    }
    
    return (page_entry & PAGE_FRAME) + (virtual_addr & 0xFFF);
}

void* get_page(uint32 virtual_addr, int make, page_directory_struct_t* dir) {
    uint32 page_dir_idx = virtual_addr / (PAGE_SIZE * PAGES_PER_TABLE);
    uint32 page_table_idx = (virtual_addr / PAGE_SIZE) % PAGES_PER_TABLE;
    
    if (dir->tables[page_dir_idx]) {
        page_table_t* table = dir->tables[page_dir_idx];
        return (void*)&((*table)[page_table_idx]);
    } else if (make) {
        uint32 phys = pmm_allocate_page();
        dir->tables[page_dir_idx] = (page_table_t*)(uintptr)phys;
        dir->physical_tables[page_dir_idx] = phys | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
        dir->directory[page_dir_idx] = phys | PAGE_PRESENT | PAGE_WRITE | PAGE_USER;
        
        memset((void*)(uintptr)phys, 0, PAGE_SIZE);
        
        page_table_t* table = dir->tables[page_dir_idx];
        return (void*)&((*table)[page_table_idx]);
    }
    
    return 0;
}

void flush_tlb_entry(uint32 virtual_addr) {
    __asm__ __volatile__("invlpg (%0)" : : "r"(virtual_addr) : "memory");
}

void flush_tlb() {
    uint64 cr3;
    __asm__ __volatile__("movq %%cr3, %0" : "=r"(cr3));
    __asm__ __volatile__("movq %0, %%cr3" : : "r"(cr3));
}

void switch_page_directory(page_directory_struct_t* dir) {
    current_directory = dir;
    uint64 phys = dir->physical_addr;
    __asm__ __volatile__("movq %0, %%cr3" : : "r"(phys));
}

void enable_paging() {
    uint64 cr0;
    __asm__ __volatile__("movq %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ __volatile__("movq %0, %%cr0" : : "r"(cr0));
}

page_directory_struct_t* clone_directory(page_directory_struct_t* src) {
    page_directory_struct_t* dir = create_page_directory();
    
    if (!dir) {
        return 0;
    }
    
    for (uint32 i = 0; i < PAGES_PER_DIR; i++) {
        if (src->tables[i]) {
            uint32 phys = pmm_allocate_page();
            dir->tables[i] = (page_table_t*)(uintptr)phys;
            dir->physical_tables[i] = src->physical_tables[i];
            dir->directory[i] = src->directory[i];
            
            for (uint32 j = 0; j < PAGES_PER_TABLE; j++) {
                page_table_t* src_table = src->tables[i];
                page_table_t* dst_table = dir->tables[i];
                (*dst_table)[j] = (*src_table)[j];
            }
        }
    }
    
    return dir;
}

void page_fault_handler(uint32 error_code) {
    uint64 faulting_address;
    __asm__ __volatile__("movq %%cr2, %0" : "=r"(faulting_address));
    
    set_screen_color(0x0C, 0x00);
    printf("Page Fault! ( ");
    
    if (!(error_code & 0x1)) {
        printf("non-present ");
    }
    if (error_code & 0x2) {
        printf("write ");
    } else {
        printf("read ");
    }
    if (error_code & 0x4) {
        printf("user-mode ");
    }
    if (error_code & 0x8) {
        printf("reserved ");
    }
    if (error_code & 0x10) {
        printf("instruction-fetch ");
    }
    
    printf(") at 0x");
    char hex_str[9];
    for (int i = 7; i >= 0; i--) {
        uint32 nibble = (faulting_address >> (i * 4)) & 0xF;
        hex_str[7-i] = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
    }
    hex_str[8] = '\0';
    printf(hex_str);
    printf("\n");
    
    set_screen_color(0x07, 0x00);
    
    while (1) {
        __asm__ __volatile__("hlt");
    }
}

void init_paging() {
    kernel_directory = create_page_directory();
    current_directory = kernel_directory;
    
    map_kernel_space();
    
    set_idt_gate(14, (uint64)page_fault_handler);
    
    switch_page_directory(kernel_directory);
    enable_paging();
}

page_directory_struct_t* get_kernel_directory() {
    return kernel_directory;
}

page_directory_struct_t* get_current_directory() {
    return current_directory;
}

void* valloc(uint32 size) {
    uint32 num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    static uint32 next_virt_addr = 0xC0000000;
    
    uint32 start_addr = next_virt_addr;
    
    for (uint32 i = 0; i < num_pages; i++) {
        uint32 phys = pmm_allocate_page();
        map_page(next_virt_addr, phys, PAGE_PRESENT | PAGE_WRITE);
        next_virt_addr += PAGE_SIZE;
    }
    
    return (void*)(uintptr)start_addr;
}

void vfree(void* ptr) {
    uintptr addr = (uintptr)ptr;
    addr &= PAGE_FRAME;
    
    uint32 phys = get_physical_address(addr);
    if (phys) {
        pmm_free_page(phys);
        unmap_page(addr);
    }
}
