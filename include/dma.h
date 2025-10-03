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

#ifndef DMA_H
#define DMA_H

#include "types.h"

#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1
#define DMA_CHANNEL_2 2
#define DMA_CHANNEL_3 3
#define DMA_CHANNEL_4 4
#define DMA_CHANNEL_5 5
#define DMA_CHANNEL_6 6
#define DMA_CHANNEL_7 7

#define DMA_MODE_READ 0x44
#define DMA_MODE_WRITE 0x48
#define DMA_MODE_AUTOINIT 0x50
#define DMA_MODE_DECREMENT 0x20

#define DMA_MAX_BUFFER_SIZE (64 * 1024)
#define DMA_REGION_LIMIT (16 * 1024 * 1024)

typedef struct dma_buffer {
    uint32 physical_addr;
    uint32 size;
    void* virtual_addr;
    int in_use;
    struct dma_buffer* next;
} dma_buffer_t;

void init_dma();
void* dma_alloc(uint32 size, uint32* phys_addr);
void dma_free(void* ptr);

void dma_set_address(uint8 channel, uint32 addr);
void dma_set_count(uint8 channel, uint16 count);
void dma_set_mode(uint8 channel, uint8 mode);
void dma_mask_channel(uint8 channel);
void dma_unmask_channel(uint8 channel);
void dma_reset_flipflop(uint8 channel);
void dma_start_transfer(uint8 channel);

void* dma_alloc_buffer(uint32 size);
void dma_free_buffer(void* buffer);
uint32 dma_get_physical_address(void* buffer);

void dma_print_stats();

#endif
