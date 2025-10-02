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

#include "../include/syscall.h"
#include "../include/idt.h"
#include "../include/timer.h"

uint32 syscall_handler(uint32 syscall_num, uint32 arg1, uint32 arg2, uint32 arg3) {
    switch(syscall_num) {
        case SYSCALL_WRITE:
            printf((char*)arg1);
            return 0;
            
        case SYSCALL_READ:
            return 0;
            
        case SYSCALL_MALLOC:
            return (uint32)kmalloc(arg1);
            
        case SYSCALL_FREE:
            kfree((void*)arg1);
            return 0;
            
        case SYSCALL_EXIT:
            terminate_process(get_current_process()->pid);
            schedule();
            return 0;
            
        case SYSCALL_GETPID:
            return get_current_process()->pid;
            
        case SYSCALL_SLEEP:
            sleep(arg1);
            return 0;
            
        default:
            return -1;
    }
}

void syscall_wrapper() {
    uint32 syscall_num, arg1, arg2, arg3;
    uint32 ret;
    
    __asm__ __volatile__(
        "mov %%eax, %0\n"
        "mov %%ebx, %1\n"
        "mov %%ecx, %2\n"
        "mov %%edx, %3\n"
        : "=r"(syscall_num), "=r"(arg1), "=r"(arg2), "=r"(arg3)
    );
    
    ret = syscall_handler(syscall_num, arg1, arg2, arg3);
    
    __asm__ __volatile__(
        "mov %0, %%eax\n"
        : : "r"(ret)
    );
}

void init_syscalls() {
    set_idt_gate(0x80, (uint32)syscall_wrapper);
}
