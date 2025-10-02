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

#include "../include/hal.h"
#include "../include/screen.h"
#include "../include/string.h"

static device_t* device_list = 0;

void init_hal() {
    device_list = 0;
}

int register_device(device_t* device) {
    if(!device) return -1;
    
    device->next = device_list;
    device_list = device;
    
    if(device->init) {
        device->init();
    }
    
    return 0;
}

device_t* get_device(const char* name) {
    device_t* current = device_list;
    
    while(current) {
        if(strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return 0;
}

void list_devices() {
    printf("Registered Devices:\n");
    
    device_t* current = device_list;
    int count = 0;
    
    while(current) {
        printf("  ");
        printf(current->name);
        printf(" (Type: ");
        
        if(current->type == DEVICE_TYPE_STORAGE) {
            printf("Storage");
        } else if(current->type == DEVICE_TYPE_INPUT) {
            printf("Input");
        } else if(current->type == DEVICE_TYPE_OUTPUT) {
            printf("Output");
        } else if(current->type == DEVICE_TYPE_NETWORK) {
            printf("Network");
        } else {
            printf("Unknown");
        }
        
        printf(")\n");
        current = current->next;
        count++;
    }
    
    if(count == 0) {
        printf("  No devices registered.\n");
    }
}
