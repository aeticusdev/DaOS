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

#include "../include/process.h"
#include "../include/screen.h"
#include "../include/string.h"
#include "../include/util.h"
#include "../include/timer.h"
#include "../include/irq.h"

static process_t processes[MAX_PROCESSES];
static uint32 current_pid = 0;
static uint32 next_pid = 1;
static int preemptive_enabled = 0;
static sched_stats_t sched_stats;

extern uint32 read_eip();

void init_process_manager() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].state = PROCESS_STATE_TERMINATED;
        processes[i].pid = 0;
        processes[i].ppid = 0;
        processes[i].priority = DEFAULT_PRIORITY;
        processes[i].time_slice = DEFAULT_TIME_SLICE;
        processes[i].quantum_used = 0;
        processes[i].total_time = 0;
        processes[i].stack = 0;
        processes[i].page_directory = 0;
    }
    
    processes[0].pid = 0;
    processes[0].ppid = 0;
    processes[0].state = PROCESS_STATE_RUNNING;
    processes[0].priority = MAX_PRIORITY;
    processes[0].time_slice = DEFAULT_TIME_SLICE * 2;
    processes[0].stack = 0;
    strcpy(processes[0].name, "kernel");
    
    sched_stats.context_switches = 0;
    sched_stats.processes_created = 1;
    sched_stats.processes_terminated = 0;
    sched_stats.total_scheduler_time = 0;
}

uint32 create_process(void (*entry_point)(), const char* name, uint32 priority) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_STATE_TERMINATED) {
            processes[i].pid = next_pid++;
            processes[i].ppid = current_pid;
            processes[i].state = PROCESS_STATE_READY;
            
            if (priority < MIN_PRIORITY) priority = MIN_PRIORITY;
            if (priority > MAX_PRIORITY) priority = MAX_PRIORITY;
            processes[i].priority = priority;
            
            processes[i].time_slice = DEFAULT_TIME_SLICE + (priority / 2);
            processes[i].quantum_used = 0;
            processes[i].total_time = 0;
            
            processes[i].stack_size = PROCESS_STACK_SIZE;
            processes[i].stack = (uint32*)kmalloc(PROCESS_STACK_SIZE);
            
            if (!processes[i].stack) {
                return 0;
            }
            
            memset(processes[i].stack, 0, PROCESS_STACK_SIZE);
            
            processes[i].cpu.eip = (uintptr)entry_point;
            processes[i].cpu.esp = (uintptr)processes[i].stack + PROCESS_STACK_SIZE - 4;
            processes[i].cpu.ebp = processes[i].cpu.esp;
            processes[i].cpu.eflags = 0x202;
            processes[i].cpu.cs = 0x08;
            processes[i].cpu.ds = 0x10;
            processes[i].cpu.es = 0x10;
            processes[i].cpu.fs = 0x10;
            processes[i].cpu.gs = 0x10;
            processes[i].cpu.ss = 0x10;
            
            processes[i].page_directory = 0;
            
            strcpy(processes[i].name, name);
            
            sched_stats.processes_created++;
            
            return processes[i].pid;
        }
    }
    return 0;
}

void terminate_process(uint32 pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid == pid) {
            if (processes[i].stack) {
                kfree(processes[i].stack);
                processes[i].stack = 0;
            }
            
            processes[i].state = PROCESS_STATE_ZOMBIE;
            processes[i].exit_code = 0;
            
            sched_stats.processes_terminated++;
            
            if (pid == current_pid) {
                schedule();
            }
            return;
        }
    }
}

void exit_process(uint32 exit_code) {
    process_t* proc = get_current_process();
    proc->exit_code = exit_code;
    terminate_process(proc->pid);
}

void yield_cpu() {
    schedule();
}

void sleep_process(uint32 ticks) {
    process_t* proc = get_current_process();
    proc->state = PROCESS_STATE_SLEEPING;
    proc->sleep_until = get_tick_count() + ticks;
    schedule();
}

static process_t* find_next_ready_process() {
    uint32 highest_priority = 0;
    int32 best_candidate = -1;
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_STATE_SLEEPING) {
            if (get_tick_count() >= processes[i].sleep_until) {
                processes[i].state = PROCESS_STATE_READY;
            }
        }
        
        if (processes[i].state == PROCESS_STATE_READY) {
            uint32 effective_priority = processes[i].priority;
            
            if (processes[i].quantum_used > 0) {
                effective_priority = effective_priority * processes[i].time_slice / 
                                   (processes[i].time_slice + processes[i].quantum_used);
            }
            
            if (effective_priority > highest_priority) {
                highest_priority = effective_priority;
                best_candidate = i;
            }
        }
    }
    
    if (best_candidate >= 0) {
        return &processes[best_candidate];
    }
    
    return &processes[0];
}

void schedule() {
    if (!preemptive_enabled && current_pid != 0) {
        return;
    }
    
    process_t* current = &processes[current_pid];
    process_t* next = find_next_ready_process();
    
    if (next == current && current->state == PROCESS_STATE_RUNNING) {
        current->quantum_used++;
        return;
    }
    
    if (current->state == PROCESS_STATE_RUNNING) {
        current->state = PROCESS_STATE_READY;
    }
    
    if (current->quantum_used >= current->time_slice) {
        current->quantum_used = 0;
    }
    
    next->state = PROCESS_STATE_RUNNING;
    next->quantum_used = 0;
    current_pid = next->pid;
    
    sched_stats.context_switches++;
}

void schedule_irq() {
    process_t* current = get_current_process();
    current->quantum_used++;
    current->total_time++;
    
    if (current->quantum_used >= current->time_slice) {
        schedule();
    }
}

process_t* get_current_process() {
    return &processes[current_pid];
}

process_t* get_process_by_pid(uint32 pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid == pid && processes[i].state != PROCESS_STATE_TERMINATED) {
            return &processes[i];
        }
    }
    return 0;
}

void set_process_priority(uint32 pid, uint32 priority) {
    process_t* proc = get_process_by_pid(pid);
    if (proc) {
        if (priority < MIN_PRIORITY) priority = MIN_PRIORITY;
        if (priority > MAX_PRIORITY) priority = MAX_PRIORITY;
        proc->priority = priority;
        proc->time_slice = DEFAULT_TIME_SLICE + (priority / 2);
    }
}

void block_process(uint32 pid) {
    process_t* proc = get_process_by_pid(pid);
    if (proc) {
        proc->state = PROCESS_STATE_BLOCKED;
        if (pid == current_pid) {
            schedule();
        }
    }
}

void unblock_process(uint32 pid) {
    process_t* proc = get_process_by_pid(pid);
    if (proc && proc->state == PROCESS_STATE_BLOCKED) {
        proc->state = PROCESS_STATE_READY;
    }
}

void enable_preemptive_scheduling() {
    preemptive_enabled = 1;
}

void disable_preemptive_scheduling() {
    preemptive_enabled = 0;
}

void list_processes() {
    printf("PID  PPID State     Pri Slice Name\n");
    printf("---  ---- --------- --- ----- ----\n");
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state != PROCESS_STATE_TERMINATED) {
            char pid_str[10];
            int_to_ascii(processes[i].pid, pid_str);
            printf(pid_str);
            printf("    ");
            
            char ppid_str[10];
            int_to_ascii(processes[i].ppid, ppid_str);
            printf(ppid_str);
            printf("    ");
            
            if (processes[i].state == PROCESS_STATE_RUNNING) {
                printf("RUNNING   ");
            } else if (processes[i].state == PROCESS_STATE_READY) {
                printf("READY     ");
            } else if (processes[i].state == PROCESS_STATE_BLOCKED) {
                printf("BLOCKED   ");
            } else if (processes[i].state == PROCESS_STATE_SLEEPING) {
                printf("SLEEPING  ");
            } else if (processes[i].state == PROCESS_STATE_ZOMBIE) {
                printf("ZOMBIE    ");
            }
            
            char prio_str[10];
            int_to_ascii(processes[i].priority, prio_str);
            printf(prio_str);
            printf("   ");
            
            char slice_str[10];
            int_to_ascii(processes[i].time_slice, slice_str);
            printf(slice_str);
            printf("     ");
            
            printf(processes[i].name);
            printf("\n");
        }
    }
}

sched_stats_t* get_scheduler_stats() {
    return &sched_stats;
}

void print_scheduler_stats() {
    printf("Scheduler Statistics:\n");
    printf("Context Switches: ");
    char ctx_str[20];
    int_to_ascii(sched_stats.context_switches, ctx_str);
    printf(ctx_str);
    printf("\n");
    
    printf("Processes Created: ");
    char created_str[20];
    int_to_ascii(sched_stats.processes_created, created_str);
    printf(created_str);
    printf("\n");
    
    printf("Processes Terminated: ");
    char term_str[20];
    int_to_ascii(sched_stats.processes_terminated, term_str);
    printf(term_str);
    printf("\n");
}
