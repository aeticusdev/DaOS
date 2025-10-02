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

#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"
#include "memory.h"

#define MAX_PROCESSES 32
#define PROCESS_STACK_SIZE 4096

#define PROCESS_STATE_READY 0
#define PROCESS_STATE_RUNNING 1
#define PROCESS_STATE_BLOCKED 2
#define PROCESS_STATE_TERMINATED 3

typedef struct {
    uint32 eax, ebx, ecx, edx;
    uint32 esi, edi, ebp;
    uint32 eip, esp;
    uint32 eflags;
} registers_t;

typedef struct {
    uint32 pid;
    uint32 state;
    uint32 priority;
    registers_t regs;
    uint32 stack[PROCESS_STACK_SIZE / 4];
    char name[32];
} process_t;

void init_process_manager();
uint32 create_process(void (*entry_point)(), const char* name, uint32 priority);
void terminate_process(uint32 pid);
void schedule();
process_t* get_current_process();
void list_processes();

#endif
