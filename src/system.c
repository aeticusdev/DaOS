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

#include "../include/system.h"

uint8 inportb (uint16 _port) {
    uint8 rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(rv) : "Nd"(_port));
    return rv;
}

void outportb (uint16 _port, uint8 _data) {
    __asm__ __volatile__ ("outb %0, %1" : : "a"(_data), "Nd"(_port));
}

uint16 inportw (uint16 _port) {
    uint16 rv;
    __asm__ __volatile__ ("inw %1, %0" : "=a"(rv) : "Nd"(_port));
    return rv;
}

void outportw (uint16 _port, uint16 _data) {
    __asm__ __volatile__ ("outw %0, %1" : : "a"(_data), "Nd"(_port));
}
