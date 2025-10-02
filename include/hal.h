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

#ifndef HAL_H
#define HAL_H

#include "types.h"

#define DEVICE_TYPE_STORAGE 1
#define DEVICE_TYPE_INPUT 2
#define DEVICE_TYPE_OUTPUT 3
#define DEVICE_TYPE_NETWORK 4

typedef struct device {
    char name[32];
    uint32 type;
    void (*init)();
    int (*read)(void* buffer, uint32 size);
    int (*write)(void* buffer, uint32 size);
    void (*close)();
    struct device* next;
} device_t;

void init_hal();
int register_device(device_t* device);
device_t* get_device(const char* name);
void list_devices();

#endif
