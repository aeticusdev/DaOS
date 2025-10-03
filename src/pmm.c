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

#include "../include/pmm.h"
#include "../include/screen.h"
#include "../include/string.h"
#include "../include/util.h"

static uint32 memory_bitmap[MEMORY_MAP_SIZE / 4];
static uint32 total_blocks;
static uint32 used_blocks;
static uint32 total_memory_size;

static inline void pmm_set_bit(uint32 bit) {
    memory_bitmap[bit / 32] |= (1 << (bit % 32));
}

static inline void pmm_clear_bit(uint32 bit) {
    memory_bitmap[bit / 32] &= ~(1 << (bit % 32));
}

static inline uint32 pmm_test_bit(uint32 bit) {
    return memory_bitmap[bit / 32] & (1 << (bit % 32));
}

static int pmm_find_first_free() {
    for (uint32 i = 0; i < total_blocks / 32; i++) {
        if (memory_bitmap[i] != 0xFFFFFFFF) {
            for (int j = 0; j < 32; j++) {
                int bit = 1 << j;
                if (!(memory_bitmap[i] & bit)) {
                    return i * 32 + j;
                }
            }
        }
    }
    return -1;
}

static int pmm_find_first_free_s(uint32 count) {
    if (count == 0) {
        return -1;
    }
    
    if (count == 1) {
        return pmm_find_first_free();
    }
    
    for (uint32 i = 0; i < total_blocks; i++) {
        int bit = pmm_test_bit(i);
        if (!bit) {
            uint32 starting_bit = i;
            uint32 free_count = 0;
            
            for (uint32 j = i; j < total_blocks; j++) {
                if (!pmm_test_bit(j)) {
                    free_count++;
                } else {
                    break;
                }
                
                if (free_count == count) {
                    return starting_bit;
                }
            }
        }
    }
    
    return -1;
}

void init_pmm(uint32 total_memory) {
    total_memory_size = total_memory;
    total_blocks = total_memory / PMM_BLOCK_SIZE;
    used_blocks = total_blocks;
    
    memset(memory_bitmap, 0xFF, sizeof(memory_bitmap));
}

void pmm_init_region(uint32 base, uint32 size) {
    uint32 align_base = base / PMM_BLOCK_SIZE;
    uint32 blocks = size / PMM_BLOCK_SIZE;
    
    for (; blocks > 0; blocks--) {
        pmm_clear_bit(align_base++);
        used_blocks--;
    }
    
    pmm_set_bit(0);
}

void pmm_deinit_region(uint32 base, uint32 size) {
    uint32 align_base = base / PMM_BLOCK_SIZE;
    uint32 blocks = size / PMM_BLOCK_SIZE;
    
    for (; blocks > 0; blocks--) {
        pmm_set_bit(align_base++);
        used_blocks++;
    }
}

uint32 pmm_allocate_page() {
    int frame = pmm_find_first_free();
    
    if (frame == -1) {
        return 0;
    }
    
    pmm_set_bit(frame);
    used_blocks++;
    
    return frame * PMM_BLOCK_SIZE;
}

void pmm_free_page(uint32 page) {
    uint32 frame = page / PMM_BLOCK_SIZE;
    pmm_clear_bit(frame);
    used_blocks--;
}

uint32 pmm_allocate_pages(uint32 count) {
    int frame = pmm_find_first_free_s(count);
    
    if (frame == -1) {
        return 0;
    }
    
    for (uint32 i = 0; i < count; i++) {
        pmm_set_bit(frame + i);
    }
    
    used_blocks += count;
    
    return frame * PMM_BLOCK_SIZE;
}

void pmm_free_pages(uint32 page, uint32 count) {
    uint32 frame = page / PMM_BLOCK_SIZE;
    
    for (uint32 i = 0; i < count; i++) {
        pmm_clear_bit(frame + i);
    }
    
    used_blocks -= count;
}

uint32 pmm_get_total_memory() {
    return total_blocks * PMM_BLOCK_SIZE;
}

uint32 pmm_get_free_memory() {
    return (total_blocks - used_blocks) * PMM_BLOCK_SIZE;
}

uint32 pmm_get_used_memory() {
    return used_blocks * PMM_BLOCK_SIZE;
}

uint32 pmm_get_block_count() {
    return total_blocks;
}

int pmm_is_page_allocated(uint32 page) {
    uint32 frame = page / PMM_BLOCK_SIZE;
    return pmm_test_bit(frame);
}

void pmm_print_stats() {
    printf("PMM Statistics:\n");
    printf("Total Memory: ");
    char total_str[20];
    int_to_ascii(pmm_get_total_memory() / 1024, total_str);
    printf(total_str);
    printf(" KB\n");
    
    printf("Used Memory: ");
    char used_str[20];
    int_to_ascii(pmm_get_used_memory() / 1024, used_str);
    printf(used_str);
    printf(" KB\n");
    
    printf("Free Memory: ");
    char free_str[20];
    int_to_ascii(pmm_get_free_memory() / 1024, free_str);
    printf(free_str);
    printf(" KB\n");
}

uint32 pmm_allocate_dma_pages(uint32 count) {
    uint32 dma_limit = 16 * 1024 * 1024;
    uint32 max_frame = dma_limit / PMM_BLOCK_SIZE;
    
    for (uint32 i = 1; i < max_frame - count; i++) {
        int found = 1;
        for (uint32 j = 0; j < count; j++) {
            if (pmm_test_bit(i + j)) {
                found = 0;
                break;
            }
        }
        
        if (found) {
            for (uint32 j = 0; j < count; j++) {
                pmm_set_bit(i + j);
            }
            used_blocks += count;
            return i * PMM_BLOCK_SIZE;
        }
    }
    
    return 0;
}
