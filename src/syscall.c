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

uint64 syscall_handler(uint64 syscall_num, uint64 arg1, uint64 arg2, uint64 arg3) {
    switch(syscall_num) {
        case SYSCALL_WRITE:
            printf((char*)(uintptr)arg1);
            return 0;
            
        case SYSCALL_READ:
            return 0;
            
        case SYSCALL_MALLOC:
            return (uintptr)kmalloc(arg1);
            
        case SYSCALL_FREE:
            kfree((void*)(uintptr)arg1);
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
    uint64 syscall_num, arg1, arg2, arg3;
    uint64 ret;
    
    __asm__ __volatile__(
        "movq %%rax, %0\n"
        "movq %%rdi, %1\n"
        "movq %%rsi, %2\n"
        "movq %%rdx, %3\n"
        : "=r"(syscall_num), "=r"(arg1), "=r"(arg2), "=r"(arg3)
    );
    
    ret = syscall_handler(syscall_num, arg1, arg2, arg3);
    
    __asm__ __volatile__(
        "movq %0, %%rax\n"
        : : "r"(ret)
    );
}

void init_syscalls() {
    set_idt_gate(0x80, (uint64)syscall_wrapper);
}
