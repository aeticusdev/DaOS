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

#ifndef FS_H
#define FS_H

#include "types.h"
#include "screen.h"
#include "string.h"
#include "memory.h"

#define MAX_FILES 32
#define MAX_FILENAME 64
#define MAX_FILESIZE 4096

typedef struct {
    char name[MAX_FILENAME];
    uint32 size;
    char data[MAX_FILESIZE];
    int is_used;
} file_t;

void init_filesystem();
int create_file(const char* filename);
int write_file(const char* filename, const char* data, uint32 size);
char* read_file(const char* filename, uint32* size);
int delete_file(const char* filename);
void list_files();
int file_exists(const char* filename);

#endif
