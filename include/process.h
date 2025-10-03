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
#define PROCESS_STACK_SIZE 8192
#define DEFAULT_TIME_SLICE 10
#define MIN_PRIORITY 1
#define MAX_PRIORITY 20
#define DEFAULT_PRIORITY 10

#define PROCESS_STATE_READY 0
#define PROCESS_STATE_RUNNING 1
#define PROCESS_STATE_BLOCKED 2
#define PROCESS_STATE_TERMINATED 3
#define PROCESS_STATE_ZOMBIE 4
#define PROCESS_STATE_SLEEPING 5

typedef struct cpu_state {
    uint32 eax, ebx, ecx, edx;
    uint32 esi, edi, ebp;
    uint32 eip, esp, ebp_saved;
    uint32 eflags;
    uint32 cr3;
    uint16 cs, ds, es, fs, gs, ss;
} cpu_state_t;

typedef struct process {
    uint32 pid;
    uint32 ppid;
    uint32 state;
    uint32 priority;
    uint32 time_slice;
    uint32 quantum_used;
    uint64 total_time;
    cpu_state_t cpu;
    uint32* stack;
    uint32 stack_size;
    void* page_directory;
    char name[32];
    uint32 sleep_until;
    uint32 exit_code;
} process_t;

typedef struct sched_stats {
    uint32 context_switches;
    uint32 processes_created;
    uint32 processes_terminated;
    uint64 total_scheduler_time;
} sched_stats_t;

void init_process_manager();
void enable_preemptive_scheduling();
void disable_preemptive_scheduling();

uint32 create_process(void (*entry_point)(), const char* name, uint32 priority);
void terminate_process(uint32 pid);
void exit_process(uint32 exit_code);
void yield_cpu();
void sleep_process(uint32 ticks);

void schedule();
void schedule_irq();
process_t* get_current_process();
process_t* get_process_by_pid(uint32 pid);

void set_process_priority(uint32 pid, uint32 priority);
void block_process(uint32 pid);
void unblock_process(uint32 pid);

void list_processes();
sched_stats_t* get_scheduler_stats();
void print_scheduler_stats();

void switch_to_process(process_t* proc);
void save_context(cpu_state_t* cpu);
void load_context(cpu_state_t* cpu);

#endif
