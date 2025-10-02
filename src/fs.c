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

#include "../include/fs.h"
#include "../include/util.h"

static file_t filesystem[MAX_FILES];
static int fs_initialized = 0;

void init_filesystem() {
    for(int i = 0; i < MAX_FILES; i++) {
        filesystem[i].is_used = 0;
        filesystem[i].size = 0;
        filesystem[i].name[0] = '\0';
    }
    fs_initialized = 1;
}

int find_file(const char* filename) {
    for(int i = 0; i < MAX_FILES; i++) {
        if(filesystem[i].is_used && strcmp(filesystem[i].name, filename) == 0) {
            return i;
        }
    }
    return -1;
}

int create_file(const char* filename) {
    if(!fs_initialized) {
        init_filesystem();
    }
    
    if(find_file(filename) != -1) {
        return -1;
    }
    
    for(int i = 0; i < MAX_FILES; i++) {
        if(!filesystem[i].is_used) {
            filesystem[i].is_used = 1;
            filesystem[i].size = 0;
            strcpy(filesystem[i].name, filename);
            return i;
        }
    }
    
    return -1;
}

int write_file(const char* filename, const char* data, uint32 size) {
    if(!fs_initialized) {
        init_filesystem();
    }
    
    int idx = find_file(filename);
    if(idx == -1) {
        idx = create_file(filename);
        if(idx == -1) {
            return -1;
        }
    }
    
    if(size > MAX_FILESIZE) {
        size = MAX_FILESIZE;
    }
    
    for(uint32 i = 0; i < size; i++) {
        filesystem[idx].data[i] = data[i];
    }
    
    filesystem[idx].size = size;
    return 0;
}

char* read_file(const char* filename, uint32* size) {
    if(!fs_initialized) {
        init_filesystem();
    }
    
    int idx = find_file(filename);
    if(idx == -1) {
        if(size) *size = 0;
        return 0;
    }
    
    if(size) {
        *size = filesystem[idx].size;
    }
    
    return filesystem[idx].data;
}

int delete_file(const char* filename) {
    if(!fs_initialized) {
        init_filesystem();
    }
    
    int idx = find_file(filename);
    if(idx == -1) {
        return -1;
    }
    
    filesystem[idx].is_used = 0;
    filesystem[idx].size = 0;
    filesystem[idx].name[0] = '\0';
    return 0;
}

void list_files() {
    if(!fs_initialized) {
        init_filesystem();
    }
    
    int count = 0;
    printf("Files in system:\n");
    
    for(int i = 0; i < MAX_FILES; i++) {
        if(filesystem[i].is_used) {
            printf("  ");
            printf(filesystem[i].name);
            printf(" (");
            char size_str[20];
            int_to_ascii(filesystem[i].size, size_str);
            printf(size_str);
            printf(" bytes)\n");
            count++;
        }
    }
    
    if(count == 0) {
        printf("  No files found.\n");
    }
}

int file_exists(const char* filename) {
    return find_file(filename) != -1;
}
