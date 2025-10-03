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

#ifndef SYSCALL_H
#define SYSCALL_H

#include "types.h"
#include "screen.h"
#include "memory.h"
#include "process.h"

#define SYSCALL_WRITE 0
#define SYSCALL_READ 1
#define SYSCALL_MALLOC 2
#define SYSCALL_FREE 3
#define SYSCALL_EXIT 4
#define SYSCALL_GETPID 5
#define SYSCALL_SLEEP 6

void init_syscalls();
uint64 syscall_handler(uint64 syscall_num, uint64 arg1, uint64 arg2, uint64 arg3);

#endif
