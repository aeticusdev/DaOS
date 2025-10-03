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

#include "../include/memory.h"
#include "../include/util.h"
#include "../include/string.h"

static memory_block_t* heap_start = 0;
static uint32 total_memory = HEAP_SIZE;
static uint32 used_memory = 0;
static alloc_strategy_t current_strategy = ALLOC_FIRST_FIT;
static heap_stats_t heap_stats;

static uint32 align_up(uint32 addr, uint32 alignment) {
    if (alignment == 0) return addr;
    return (addr + alignment - 1) & ~(alignment - 1);
}

void init_memory() {
    heap_start = (memory_block_t*)HEAP_START;
    heap_start->magic = HEAP_MAGIC;
    heap_start->size = HEAP_SIZE - sizeof(memory_block_t);
    heap_start->is_free = 1;
    heap_start->next = 0;
    heap_start->prev = 0;
    
    heap_stats.total_size = HEAP_SIZE;
    heap_stats.used_size = 0;
    heap_stats.free_size = HEAP_SIZE;
    heap_stats.num_blocks = 1;
    heap_stats.num_allocs = 0;
    heap_stats.num_frees = 0;
    heap_stats.num_coalesces = 0;
}

static memory_block_t* find_block_first_fit(uint32 size) {
    memory_block_t* current = heap_start;
    
    while (current) {
        if (current->is_free && current->size >= size && current->magic == HEAP_MAGIC) {
            return current;
        }
        current = current->next;
    }
    
    return 0;
}

static memory_block_t* find_block_best_fit(uint32 size) {
    memory_block_t* current = heap_start;
    memory_block_t* best = 0;
    uint32 best_size = 0xFFFFFFFF;
    
    while (current) {
        if (current->is_free && current->size >= size && current->magic == HEAP_MAGIC) {
            if (current->size < best_size) {
                best = current;
                best_size = current->size;
            }
        }
        current = current->next;
    }
    
    return best;
}

static memory_block_t* find_block_worst_fit(uint32 size) {
    memory_block_t* current = heap_start;
    memory_block_t* worst = 0;
    uint32 worst_size = 0;
    
    while (current) {
        if (current->is_free && current->size >= size && current->magic == HEAP_MAGIC) {
            if (current->size > worst_size) {
                worst = current;
                worst_size = current->size;
            }
        }
        current = current->next;
    }
    
    return worst;
}

static memory_block_t* find_block(uint32 size) {
    switch (current_strategy) {
        case ALLOC_FIRST_FIT:
            return find_block_first_fit(size);
        case ALLOC_BEST_FIT:
            return find_block_best_fit(size);
        case ALLOC_WORST_FIT:
            return find_block_worst_fit(size);
        default:
            return find_block_first_fit(size);
    }
}

static void split_block(memory_block_t* block, uint32 size) {
    if (block->size > size + sizeof(memory_block_t) + MIN_ALLOC_SIZE) {
        memory_block_t* new_block = (memory_block_t*)((uintptr)block + sizeof(memory_block_t) + size);
        new_block->magic = HEAP_MAGIC;
        new_block->size = block->size - size - sizeof(memory_block_t);
        new_block->is_free = 1;
        new_block->next = block->next;
        new_block->prev = block;
        
        if (block->next) {
            block->next->prev = new_block;
        }
        
        block->size = size;
        block->next = new_block;
        
        heap_stats.num_blocks++;
    }
}

void* kmalloc(size_t size) {
    if (!heap_start) {
        init_memory();
    }
    
    if (size == 0) {
        return 0;
    }
    
    size = align_up(size, ALIGNMENT);
    
    memory_block_t* block = find_block(size);
    
    if (!block) {
        return 0;
    }
    
    split_block(block, size);
    
    block->is_free = 0;
    used_memory += block->size + sizeof(memory_block_t);
    
    heap_stats.used_size += block->size + sizeof(memory_block_t);
    heap_stats.free_size -= block->size + sizeof(memory_block_t);
    heap_stats.num_allocs++;
    
    return (void*)((uintptr)block + sizeof(memory_block_t));
}
void* kmalloc_a(size_t size, size_t alignment) {
    if (!heap_start) {
        init_memory();
    }
    
    if (size == 0) {
        return 0;
    }
    
    size = align_up(size, ALIGNMENT);
    uint32 aligned_size = size + alignment + sizeof(memory_block_t);
    
    void* ptr = kmalloc(aligned_size);
    if (!ptr) {
        return 0;
    }
    
    uintptr addr = (uintptr)ptr;
    uintptr aligned_addr = align_up(addr, alignment);
    
    return (void*)aligned_addr;
}

void* kcalloc(size_t num, size_t size) {
    size_t total_size = num * size;
    void* ptr = kmalloc(total_size);
    
    if (ptr) {
        memset(ptr, 0, total_size);
    }
    
    return ptr;
}

void* krealloc(void* ptr, size_t size) {
    if (!ptr) {
        return kmalloc(size);
    }
    
    if (size == 0) {
        kfree(ptr);
        return 0;
    }
    
    memory_block_t* block = (memory_block_t*)((uintptr)ptr - sizeof(memory_block_t));
    
    if (block->magic != HEAP_MAGIC) {
        return 0;
    }
    
    if (block->size >= size) {
        return ptr;
    }
    
    void* new_ptr = kmalloc(size);
    if (!new_ptr) {
        return 0;
    }
    
    memcpy(new_ptr, ptr, block->size);
    kfree(ptr);
    
    return new_ptr;
}

static void coalesce_blocks() {
    memory_block_t* current = heap_start;
    
    while (current && current->next) {
        if (current->is_free && current->next->is_free && 
            current->magic == HEAP_MAGIC && current->next->magic == HEAP_MAGIC) {
            current->size += sizeof(memory_block_t) + current->next->size;
            current->next = current->next->next;
            
            if (current->next) {
                current->next->prev = current;
            }
            
            heap_stats.num_blocks--;
            heap_stats.num_coalesces++;
        } else {
            current = current->next;
        }
    }
}

void kfree(void* ptr) {
    if (!ptr) {
        return;
    }
    
    memory_block_t* block = (memory_block_t*)((uintptr)ptr - sizeof(memory_block_t));
    
    if (block->magic != HEAP_MAGIC) {
        return;
    }
    
    if (block->is_free) {
        return;
    }
    
    block->is_free = 1;
    used_memory -= block->size + sizeof(memory_block_t);
    
    heap_stats.used_size -= block->size + sizeof(memory_block_t);
    heap_stats.free_size += block->size + sizeof(memory_block_t);
    heap_stats.num_frees++;
    
    coalesce_blocks();
}

void set_alloc_strategy(alloc_strategy_t strategy) {
    current_strategy = strategy;
}

void print_memory_stats() {
    printf("Memory Statistics:\n");
    printf("Total Memory: ");
    char total_str[20];
    int_to_ascii(heap_stats.total_size / 1024, total_str);
    printf(total_str);
    printf(" KB\n");
    
    printf("Used Memory: ");
    char used_str[20];
    int_to_ascii(heap_stats.used_size / 1024, used_str);
    printf(used_str);
    printf(" KB\n");
    
    printf("Free Memory: ");
    char free_str[20];
    int_to_ascii(heap_stats.free_size / 1024, free_str);
    printf(free_str);
    printf(" KB\n");
    
    printf("Allocations: ");
    char alloc_str[20];
    int_to_ascii(heap_stats.num_allocs, alloc_str);
    printf(alloc_str);
    printf("\n");
    
    printf("Frees: ");
    char free_count_str[20];
    int_to_ascii(heap_stats.num_frees, free_count_str);
    printf(free_count_str);
    printf("\n");
    
    printf("Coalesces: ");
    char coal_str[20];
    int_to_ascii(heap_stats.num_coalesces, coal_str);
    printf(coal_str);
    printf("\n");
}

void print_heap_blocks() {
    memory_block_t* current = heap_start;
    int block_num = 0;
    
    printf("Heap Blocks:\n");
    while (current && block_num < 20) {
        printf("Block ");
        char num_str[10];
        int_to_ascii(block_num, num_str);
        printf(num_str);
        printf(": Size=");
        
        char size_str[20];
        int_to_ascii(current->size, size_str);
        printf(size_str);
        printf(" ");
        
        if (current->is_free) {
            printf("FREE\n");
        } else {
            printf("USED\n");
        }
        
        current = current->next;
        block_num++;
    }
}

uint32 get_free_memory() {
    return heap_stats.free_size;
}

uint32 get_used_memory() {
    return heap_stats.used_size;
}

heap_stats_t* get_heap_stats() {
    return &heap_stats;
}

void* kmalloc_p(size_t size, uint32* phys) {
    void* ptr = kmalloc(size);
    if (ptr && phys) {
        *phys = (uintptr)ptr;
    }
    return ptr;
}

void* kmalloc_ap(size_t size, size_t alignment, uint32* phys) {
    void* ptr = kmalloc_a(size, alignment);
    if (ptr && phys) {
        *phys = (uintptr)ptr;
    }
    return ptr;
}
