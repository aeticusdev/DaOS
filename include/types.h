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

#ifndef TYPES_H
#define TYPES_H

typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int64;
typedef unsigned long long uint64;
typedef char* string;

#ifdef __x86_64__
typedef uint64 uintptr;
typedef int64 intptr;
typedef uint64 size_t;
#else
typedef uint32 uintptr;
typedef int32 intptr;
typedef uint32 size_t;
#endif

#define low_16(address) (uint16)((address) & 0xFFFF)            
#define high_16(address) (uint16)(((address) >> 16) & 0xFFFF)
#define low_32(address) (uint32)((address) & 0xFFFFFFFF)
#define high_32(address) (uint32)(((address) >> 32) & 0xFFFFFFFF)

#endif