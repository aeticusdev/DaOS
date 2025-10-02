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

void kmain() {
    clearScreen();
    set_screen_color(0x0B, 0x00);
    printf("=================================\n");
    printf("   DaOS Kernel v1.0 Booting...   \n");
    printf("=================================\n\n");
    set_screen_color(0x0F, 0x00);
    
    printf("[1/8] Initializing IDT...\n");
    set_idt();
    
    printf("[2/8] Installing ISRs...\n");
    isr_install();
    
    printf("[3/8] Remapping and Installing IRQs...\n");
    irq_remap();
    irq_install();
    
    printf("[4/9] Initializing Timer (100Hz)...\n");
    init_timer(100);
    
    printf("[5/9] Initializing Memory Manager...\n");
    init_memory();
    
    printf("[6/9] Initializing Process Manager...\n");
    init_process_manager();
    
    printf("[7/9] Initializing System Calls...\n");
    init_syscalls();
    
    printf("[8/9] Initializing HAL...\n");
    init_hal();
    
    printf("[9/9] Initializing Filesystem...\n");
    init_filesystem();
    
    set_screen_color(0x0A, 0x00);
    printf("\n[OK] All systems initialized successfully!\n");
    printf("[OK] Enabling hardware interrupts...\n\n");
    set_screen_color(0x0F, 0x00);
    
    __asm__ __volatile__("sti");
    
    printf("Type 'help' for available commands.\n\n");
    launch_shell(0);
    
    while (1) {
        __asm__ __volatile__("hlt");
    }
}