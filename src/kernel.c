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

void kmain() {
    clearScreen();
    set_screen_color(0x0F, 0x00);
    printf("DaOS Kernel Initialized.\n");
    isr_install();
    printf("Interrupt Service Routines Installed.\n");
    printf("Launching Shell...\n\n");
    launch_shell(0);
    while (1);
}