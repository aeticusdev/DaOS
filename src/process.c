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

static process_t processes[MAX_PROCESSES];
static uint32 current_pid = 0;
static uint32 next_pid = 1;

void init_process_manager() {
    for(int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].state = PROCESS_STATE_TERMINATED;
        processes[i].pid = 0;
    }
    
    processes[0].pid = 0;
    processes[0].state = PROCESS_STATE_RUNNING;
    processes[0].priority = 10;
    strcpy(processes[0].name, "kernel");
}

uint32 create_process(void (*entry_point)(), const char* name, uint32 priority) {
    for(int i = 0; i < MAX_PROCESSES; i++) {
        if(processes[i].state == PROCESS_STATE_TERMINATED) {
            processes[i].pid = next_pid++;
            processes[i].state = PROCESS_STATE_READY;
            processes[i].priority = priority;
            processes[i].regs.eip = (uint32)entry_point;
            processes[i].regs.esp = (uint32)&processes[i].stack[PROCESS_STACK_SIZE / 4 - 1];
            strcpy(processes[i].name, name);
            return processes[i].pid;
        }
    }
    return 0;
}

void terminate_process(uint32 pid) {
    for(int i = 0; i < MAX_PROCESSES; i++) {
        if(processes[i].pid == pid) {
            processes[i].state = PROCESS_STATE_TERMINATED;
            return;
        }
    }
}

void schedule() {
    uint32 next = (current_pid + 1) % MAX_PROCESSES;
    
    while(next != current_pid) {
        if(processes[next].state == PROCESS_STATE_READY) {
            processes[current_pid].state = PROCESS_STATE_READY;
            processes[next].state = PROCESS_STATE_RUNNING;
            current_pid = next;
            return;
        }
        next = (next + 1) % MAX_PROCESSES;
    }
}

process_t* get_current_process() {
    return &processes[current_pid];
}

void list_processes() {
    printf("PID  State    Priority Name\n");
    printf("---  -------  -------- ----\n");
    
    for(int i = 0; i < MAX_PROCESSES; i++) {
        if(processes[i].state != PROCESS_STATE_TERMINATED) {
            char pid_str[10];
            int_to_ascii(processes[i].pid, pid_str);
            printf(pid_str);
            printf("    ");
            
            if(processes[i].state == PROCESS_STATE_RUNNING) {
                printf("RUNNING ");
            } else if(processes[i].state == PROCESS_STATE_READY) {
                printf("READY   ");
            } else if(processes[i].state == PROCESS_STATE_BLOCKED) {
                printf("BLOCKED ");
            }
            
            printf(" ");
            char prio_str[10];
            int_to_ascii(processes[i].priority, prio_str);
            printf(prio_str);
            printf("        ");
            printf(processes[i].name);
            printf("\n");
        }
    }
}
