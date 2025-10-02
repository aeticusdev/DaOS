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

static memory_block_t* heap_start = 0;
static uint32 total_memory = HEAP_SIZE;
static uint32 used_memory = 0;

void init_memory() {
    heap_start = (memory_block_t*)HEAP_START;
    heap_start->size = HEAP_SIZE - sizeof(memory_block_t);
    heap_start->is_free = 1;
    heap_start->next = 0;
}

void* kmalloc(uint32 size) {
    if(!heap_start) {
        init_memory();
    }
    
    memory_block_t* current = heap_start;
    
    while(current) {
        if(current->is_free && current->size >= size) {
            if(current->size > size + sizeof(memory_block_t) + MEM_BLOCK_SIZE) {
                memory_block_t* new_block = (memory_block_t*)((uint32)current + sizeof(memory_block_t) + size);
                new_block->size = current->size - size - sizeof(memory_block_t);
                new_block->is_free = 1;
                new_block->next = current->next;
                
                current->size = size;
                current->next = new_block;
            }
            
            current->is_free = 0;
            used_memory += current->size + sizeof(memory_block_t);
            return (void*)((uint32)current + sizeof(memory_block_t));
        }
        current = current->next;
    }
    
    return 0;
}

void kfree(void* ptr) {
    if(!ptr) {
        return;
    }
    
    memory_block_t* block = (memory_block_t*)((uint32)ptr - sizeof(memory_block_t));
    block->is_free = 1;
    used_memory -= block->size + sizeof(memory_block_t);
    
    memory_block_t* current = heap_start;
    while(current && current->next) {
        if(current->is_free && current->next->is_free) {
            current->size += sizeof(memory_block_t) + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

void print_memory_stats() {
    printf("Memory Statistics:\n");
    printf("Total Memory: ");
    char total_str[20];
    int_to_ascii(total_memory, total_str);
    printf(total_str);
    printf(" bytes\n");
    
    printf("Used Memory: ");
    char used_str[20];
    int_to_ascii(used_memory, used_str);
    printf(used_str);
    printf(" bytes\n");
    
    printf("Free Memory: ");
    char free_str[20];
    int_to_ascii(total_memory - used_memory, free_str);
    printf(free_str);
    printf(" bytes\n");
}

uint32 get_free_memory() {
    return total_memory - used_memory;
}

uint32 get_used_memory() {
    return used_memory;
}
