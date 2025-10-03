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

#include "../include/dma.h"
#include "../include/pmm.h"
#include "../include/memory.h"
#include "../include/system.h"
#include "../include/screen.h"
#include "../include/util.h"

static dma_buffer_t* dma_buffer_list = 0;
static uint32 dma_buffers_allocated = 0;

static const uint16 dma_port_addr[] = {0x00, 0x02, 0x04, 0x06, 0xC0, 0xC4, 0xC8, 0xCC};
static const uint16 dma_port_count[] = {0x01, 0x03, 0x05, 0x07, 0xC2, 0xC6, 0xCA, 0xCE};
static const uint16 dma_port_page[] = {0x87, 0x83, 0x81, 0x82, 0x8F, 0x8B, 0x89, 0x8A};
static const uint16 dma_port_mask[] = {0x0A, 0x0A, 0x0A, 0x0A, 0xD4, 0xD4, 0xD4, 0xD4};
static const uint16 dma_port_mode[] = {0x0B, 0x0B, 0x0B, 0x0B, 0xD6, 0xD6, 0xD6, 0xD6};
static const uint16 dma_port_clear[] = {0x0C, 0x0C, 0x0C, 0x0C, 0xD8, 0xD8, 0xD8, 0xD8};

void init_dma() {
    dma_buffer_list = 0;
    dma_buffers_allocated = 0;
    
    for (int i = 0; i < 8; i++) {
        if (i != 4) {
            dma_mask_channel(i);
        }
    }
}

void* dma_alloc(uint32 size, uint32* phys_addr) {
    if (size == 0 || size > DMA_MAX_BUFFER_SIZE) {
        return 0;
    }
    
    uint32 pages = (size + 0xFFF) / 0x1000;
    uint32 phys = pmm_allocate_dma_pages(pages);
    
    if (phys == 0 || phys >= DMA_REGION_LIMIT) {
        return 0;
    }
    
    dma_buffer_t* buffer = (dma_buffer_t*)kmalloc(sizeof(dma_buffer_t));
    if (!buffer) {
        return 0;
    }
    
    buffer->physical_addr = phys;
    buffer->size = size;
    buffer->virtual_addr = (void*)phys;
    buffer->in_use = 1;
    buffer->next = dma_buffer_list;
    
    dma_buffer_list = buffer;
    dma_buffers_allocated++;
    
    if (phys_addr) {
        *phys_addr = phys;
    }
    
    return (void*)phys;
}

void dma_free(void* ptr) {
    if (!ptr) {
        return;
    }
    
    dma_buffer_t* current = dma_buffer_list;
    dma_buffer_t* prev = 0;
    
    while (current) {
        if (current->virtual_addr == ptr) {
            uint32 pages = (current->size + 0xFFF) / 0x1000;
            pmm_free_pages(current->physical_addr, pages);
            
            if (prev) {
                prev->next = current->next;
            } else {
                dma_buffer_list = current->next;
            }
            
            kfree(current);
            dma_buffers_allocated--;
            return;
        }
        
        prev = current;
        current = current->next;
    }
}

void dma_set_address(uint8 channel, uint32 addr) {
    if (channel > 7) {
        return;
    }
    
    uint8 low = addr & 0xFF;
    uint8 mid = (addr >> 8) & 0xFF;
    uint8 high = (addr >> 16) & 0xFF;
    
    outportb(dma_port_addr[channel], low);
    outportb(dma_port_addr[channel], mid);
    outportb(dma_port_page[channel], high);
}

void dma_set_count(uint8 channel, uint16 count) {
    if (channel > 7) {
        return;
    }
    
    uint8 low = count & 0xFF;
    uint8 high = (count >> 8) & 0xFF;
    
    outportb(dma_port_count[channel], low);
    outportb(dma_port_count[channel], high);
}

void dma_set_mode(uint8 channel, uint8 mode) {
    if (channel > 7) {
        return;
    }
    
    outportb(dma_port_mode[channel], mode | (channel % 4));
}

void dma_mask_channel(uint8 channel) {
    if (channel > 7) {
        return;
    }
    
    outportb(dma_port_mask[channel], 0x04 | (channel % 4));
}

void dma_unmask_channel(uint8 channel) {
    if (channel > 7) {
        return;
    }
    
    outportb(dma_port_mask[channel], channel % 4);
}

void dma_reset_flipflop(uint8 channel) {
    if (channel > 7) {
        return;
    }
    
    outportb(dma_port_clear[channel], 0xFF);
}

void dma_start_transfer(uint8 channel) {
    if (channel > 7) {
        return;
    }
    
    dma_unmask_channel(channel);
}

void* dma_alloc_buffer(uint32 size) {
    return dma_alloc(size, 0);
}

void dma_free_buffer(void* buffer) {
    dma_free(buffer);
}

uint32 dma_get_physical_address(void* buffer) {
    if (!buffer) {
        return 0;
    }
    
    dma_buffer_t* current = dma_buffer_list;
    
    while (current) {
        if (current->virtual_addr == buffer) {
            return current->physical_addr;
        }
        current = current->next;
    }
    
    return 0;
}

void dma_print_stats() {
    printf("DMA Statistics:\n");
    printf("Allocated Buffers: ");
    char count_str[20];
    int_to_ascii(dma_buffers_allocated, count_str);
    printf(count_str);
    printf("\n");
    
    uint32 total_size = 0;
    dma_buffer_t* current = dma_buffer_list;
    
    while (current) {
        total_size += current->size;
        current = current->next;
    }
    
    printf("Total DMA Memory: ");
    char size_str[20];
    int_to_ascii(total_size / 1024, size_str);
    printf(size_str);
    printf(" KB\n");
}
