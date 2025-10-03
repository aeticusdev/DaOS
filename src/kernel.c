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

#include "../include/kb.h"
#include "../include/shell.h"
#include "../include/util.h"
#include "../include/screen.h"
#include "../include/isr.h"
#include "../include/idt.h"
#include "../include/irq.h"
#include "../include/memory.h"
#include "../include/fs.h"
#include "../include/timer.h"
#include "../include/process.h"
#include "../include/syscall.h"
#include "../include/hal.h"
#include "../include/pmm.h"
#include "../include/paging.h"
#include "../include/dma.h"
#include "../include/disk.h"
#include "../include/ext2.h"

void kmain() {
    clearScreen();
    set_screen_color(0x0B, 0x00);
    printf("=================================\n");
    printf("   DaOS Kernel v2.0 Booting...   \n");
    printf("=================================\n\n");
    set_screen_color(0x0F, 0x00);
    
    printf("[1/14] Initializing IDT...\n");
    set_idt();
    
    printf("[2/14] Installing ISRs...\n");
    isr_install();
    
    printf("[3/14] Remapping and Installing IRQs...\n");
    irq_remap();
    irq_install();
    
    printf("[4/14] Initializing Physical Memory Manager...\n");
    init_pmm(0x1000000);
    pmm_init_region(0x400000, 0xC00000);
    
    printf("[5/14] Initializing Virtual Memory Manager...\n");
    init_memory();
    
    printf("[6/14] Initializing Paging (DISABLED for debugging)...\n");
    printf("  Paging initialization skipped\n");
    
    printf("[7/14] Initializing DMA Allocator...\n");
    init_dma();
    
    printf("[8/14] Initializing Timer (100Hz)...\n");
    init_timer(100);
    
    printf("[9/14] Initializing Process Manager...\n");
    init_process_manager();
    
    printf("[10/14] Initializing System Calls...\n");
    init_syscalls();
    
    printf("[11/14] Initializing HAL...\n");
    init_hal();
    
    printf("[12/14] Initializing Simple Filesystem...\n");
    init_filesystem();
    
    printf("[13/14] Initializing ATA Disk Driver...\n");
    init_disk();
    disk_print_info();
    
    printf("[14/14] Mounting EXT2 Filesystem...\n");
    ext2_init(0, 0);
    int mount_result = ext2_mount();
    
    printf("Mount returned: ");
    char result_str[10];
    int_to_ascii(mount_result, result_str);
    printf(result_str);
    printf("\n");
    
    if (mount_result == 0) {
        printf("  EXT2 filesystem mounted successfully!\n");
    } else {
        printf("  Warning: Could not mount EXT2 filesystem\n");
    }
    
    set_screen_color(0x0A, 0x00);
    printf("\n[OK] All systems initialized successfully!\n");
    printf("[OK] Enabling hardware interrupts...\n");
    set_screen_color(0x0F, 0x00);
    
    __asm__ __volatile__("sti");
    
    printf("Interrupts enabled\n");
    printf("Type 'help' for available commands.\n\n");
    
    printf("Starting shell...\n");
    launch_shell(0);
    printf("Shell exited\n");
    
    while (1) {
        __asm__ __volatile__("hlt");
    }
}