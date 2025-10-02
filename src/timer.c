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

#include "../include/timer.h"
#include "../include/irq.h"
#include "../include/system.h"

static uint32 tick = 0;

void timer_handler() {
    tick++;
}

void init_timer(uint32 frequency) {
    uint32 divisor = 1193180 / frequency;
    
    outportb(0x43, 0x36);
    
    uint8 l = (uint8)(divisor & 0xFF);
    uint8 h = (uint8)((divisor >> 8) & 0xFF);
    
    outportb(0x40, l);
    outportb(0x40, h);
    
    irq_set_handler(0, timer_handler);
}

uint32 get_tick_count() {
    return tick;
}

void sleep(uint32 milliseconds) {
    uint32 start = tick;
    uint32 target = start + milliseconds;
    
    while(tick < target) {
        __asm__ __volatile__("hlt");
    }
}
