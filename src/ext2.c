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

#include "../include/ext2.h"
#include "../include/memory.h"
#include "../include/string.h"
#include "../include/screen.h"
#include "../include/util.h"

static ext2_fs_t filesystem;
static int ext2_mounted = 0;

void ext2_init(uint8 drive, uint32 partition_offset) {
    filesystem.drive = drive;
    filesystem.partition_offset = partition_offset;
    ext2_mounted = 0;
}

int ext2_mount() {
    uint8* buffer = (uint8*)kmalloc(SECTOR_SIZE * 2);
    if (!buffer) {
        printf("  [EXT2] Failed to allocate buffer\n");
        return -1;
    }
    
    int result = disk_read_sectors(filesystem.drive, filesystem.partition_offset + 2, 2, buffer);
    if (result != 0) {
        printf("  [EXT2] Failed to read sectors (error: ");
        char err[10];
        int_to_ascii(result, err);
        printf(err);
        printf(")\n");
        kfree(buffer);
        return -2;
    }
    
    memcpy(&filesystem.superblock, buffer, sizeof(ext2_superblock_t));
    kfree(buffer);
    
    printf("  [EXT2] Read superblock, magic=0x");
    char magic_str[10];
    uint_to_hex(filesystem.superblock.s_magic, magic_str);
    printf(magic_str);
    printf(" (expected 0xEF53)\n");
    
    if (filesystem.superblock.s_magic != EXT2_SUPER_MAGIC) {
        printf("  [EXT2] Invalid magic number\n");
        return -3;
    }
    
    if (filesystem.superblock.s_log_block_size > 10) {
        printf("  [EXT2] Invalid block size\n");
        return -4;
    }
    
    filesystem.block_size = 1024 << filesystem.superblock.s_log_block_size;
    filesystem.num_block_groups = (filesystem.superblock.s_blocks_count + filesystem.superblock.s_blocks_per_group - 1) / filesystem.superblock.s_blocks_per_group;
    filesystem.inodes_per_group = filesystem.superblock.s_inodes_per_group;
    filesystem.blocks_per_group = filesystem.superblock.s_blocks_per_group;
    
    printf("  [EXT2] Block size: ");
    char bs_str[20];
    int_to_ascii(filesystem.block_size, bs_str);
    printf(bs_str);
    printf(" bytes, Block groups: ");
    int_to_ascii(filesystem.num_block_groups, bs_str);
    printf(bs_str);
    printf("\n");
    
    if (filesystem.num_block_groups > 1000 || filesystem.num_block_groups == 0) {
        printf("  [EXT2] Invalid number of block groups\n");
        return -5;
    }
    
    uint32 bgd_blocks = (filesystem.num_block_groups * sizeof(ext2_bgd_t) + filesystem.block_size - 1) / filesystem.block_size;
    uint32 bgd_start = filesystem.superblock.s_first_data_block + 1;
    
    printf("  [EXT2] BGD blocks needed: ");
    char bgd_str[10];
    int_to_ascii(bgd_blocks, bgd_str);
    printf(bgd_str);
    printf("\n");
    
    printf("  [EXT2] Allocating space for block group descriptors...\n");
    uint32 bgd_alloc_size = filesystem.num_block_groups * sizeof(ext2_bgd_t);
    printf("  [EXT2] Size: ");
    int_to_ascii(bgd_alloc_size, bgd_str);
    printf(bgd_str);
    printf(" bytes\n");
    
    filesystem.block_groups = (ext2_bgd_t*)kmalloc(bgd_alloc_size);
    if (!filesystem.block_groups) {
        printf("  [EXT2] Failed to allocate block_groups\n");
        return -6;
    }
    printf("  [EXT2] Block groups allocated OK\n");
    
    printf("  [EXT2] Allocating BGD buffer...\n");
    uint32 bgd_buffer_size = bgd_blocks * filesystem.block_size;
    printf("  [EXT2] Buffer size: ");
    int_to_ascii(bgd_buffer_size, bgd_str);
    printf(bgd_str);
    printf(" bytes\n");
    
    if (bgd_buffer_size > 16384) {
        printf("  [EXT2] Buffer too large, limiting to 16KB\n");
        bgd_buffer_size = 16384;
        bgd_blocks = bgd_buffer_size / filesystem.block_size;
    }
    
    uint8* bgd_buffer = (uint8*)kmalloc(bgd_buffer_size);
    if (!bgd_buffer) {
        printf("  [EXT2] Failed to allocate bgd_buffer\n");
        kfree(filesystem.block_groups);
        return -7;
    }
    printf("  [EXT2] BGD buffer allocated OK\n");
    
    printf("  [EXT2] Reading BGD blocks...\n");
    uint32 sectors_per_block = filesystem.block_size / SECTOR_SIZE;
    
    if (bgd_blocks > 2) {
        printf("  [EXT2] Warning: Too many BGD blocks, limiting to 2\n");
        bgd_blocks = 2;
    }
    
    for (uint32 i = 0; i < bgd_blocks; i++) {
        printf("    Block ");
        char num[10];
        int_to_ascii(i, num);
        printf(num);
        printf("...");
        
        int read_result = disk_read_sectors(filesystem.drive, filesystem.partition_offset + (bgd_start + i) * sectors_per_block, sectors_per_block, bgd_buffer + i * filesystem.block_size);
        if (read_result != 0) {
            printf(" FAILED\n");
            kfree(bgd_buffer);
            kfree(filesystem.block_groups);
            return -8;
        }
        printf(" OK\n");
    }
    
    printf("  [EXT2] Copying BGD data...\n");
    
    memcpy(filesystem.block_groups, bgd_buffer, filesystem.num_block_groups * sizeof(ext2_bgd_t));
    
    printf("  [EXT2] Freeing BGD buffer...\n");
    kfree(bgd_buffer);
    
    printf("  [EXT2] Mount complete!\n");
    ext2_mounted = 1;
    return 0;
}

void ext2_unmount() {
    if (ext2_mounted && filesystem.block_groups) {
        kfree(filesystem.block_groups);
        filesystem.block_groups = 0;
    }
    ext2_mounted = 0;
}

ext2_inode_t* ext2_read_inode(uint32 inode_num) {
    if (!ext2_mounted || inode_num == 0) {
        return 0;
    }
    
    uint32 block_group = (inode_num - 1) / filesystem.inodes_per_group;
    uint32 index = (inode_num - 1) % filesystem.inodes_per_group;
    uint32 block_offset = (index * filesystem.superblock.s_inode_size) / filesystem.block_size;
    uint32 offset_in_block = (index * filesystem.superblock.s_inode_size) % filesystem.block_size;
    
    uint32 inode_table_block = filesystem.block_groups[block_group].bg_inode_table + block_offset;
    
    uint8* buffer = (uint8*)kmalloc(filesystem.block_size);
    if (!buffer) {
        return 0;
    }
    
    uint32 sectors_per_block = filesystem.block_size / SECTOR_SIZE;
    disk_read_sectors(filesystem.drive, filesystem.partition_offset + inode_table_block * sectors_per_block, sectors_per_block, buffer);
    
    ext2_inode_t* inode = (ext2_inode_t*)kmalloc(sizeof(ext2_inode_t));
    if (!inode) {
        kfree(buffer);
        return 0;
    }
    
    memcpy(inode, buffer + offset_in_block, sizeof(ext2_inode_t));
    kfree(buffer);
    
    return inode;
}

int ext2_write_inode(uint32 inode_num, ext2_inode_t* inode) {
    if (!ext2_mounted || inode_num == 0 || !inode) {
        return -1;
    }
    
    uint32 block_group = (inode_num - 1) / filesystem.inodes_per_group;
    uint32 index = (inode_num - 1) % filesystem.inodes_per_group;
    uint32 block_offset = (index * filesystem.superblock.s_inode_size) / filesystem.block_size;
    uint32 offset_in_block = (index * filesystem.superblock.s_inode_size) % filesystem.block_size;
    
    uint32 inode_table_block = filesystem.block_groups[block_group].bg_inode_table + block_offset;
    
    uint8* buffer = (uint8*)kmalloc(filesystem.block_size);
    if (!buffer) {
        return -1;
    }
    
    uint32 sectors_per_block = filesystem.block_size / SECTOR_SIZE;
    disk_read_sectors(filesystem.drive, filesystem.partition_offset + inode_table_block * sectors_per_block, sectors_per_block, buffer);
    
    memcpy(buffer + offset_in_block, inode, sizeof(ext2_inode_t));
    
    disk_write_sectors(filesystem.drive, filesystem.partition_offset + inode_table_block * sectors_per_block, sectors_per_block, buffer);
    
    kfree(buffer);
    return 0;
}

uint32 ext2_read_inode_block(ext2_inode_t* inode, uint32 block_index) {
    if (!inode) {
        return 0;
    }
    
    uint32 entries_per_block = filesystem.block_size / sizeof(uint32);
    
    if (block_index < EXT2_DIRECT_BLOCKS) {
        return inode->i_block[block_index];
    }
    
    block_index -= EXT2_DIRECT_BLOCKS;
    
    if (block_index < entries_per_block) {
        uint32* indirect_block = (uint32*)kmalloc(filesystem.block_size);
        if (!indirect_block) {
            return 0;
        }
        
        uint32 sectors_per_block = filesystem.block_size / SECTOR_SIZE;
        disk_read_sectors(filesystem.drive, filesystem.partition_offset + inode->i_block[EXT2_DIRECT_BLOCKS] * sectors_per_block, sectors_per_block, (uint8*)indirect_block);
        
        uint32 result = indirect_block[block_index];
        kfree(indirect_block);
        return result;
    }
    
    block_index -= entries_per_block;
    
    if (block_index < entries_per_block * entries_per_block) {
        uint32* indirect_block = (uint32*)kmalloc(filesystem.block_size);
        if (!indirect_block) {
            return 0;
        }
        
        uint32 sectors_per_block = filesystem.block_size / SECTOR_SIZE;
        disk_read_sectors(filesystem.drive, filesystem.partition_offset + inode->i_block[EXT2_DIRECT_BLOCKS + 1] * sectors_per_block, sectors_per_block, (uint8*)indirect_block);
        
        uint32 indirect_index = block_index / entries_per_block;
        uint32 indirect_block_num = indirect_block[indirect_index];
        
        disk_read_sectors(filesystem.drive, filesystem.partition_offset + indirect_block_num * sectors_per_block, sectors_per_block, (uint8*)indirect_block);
        
        uint32 result = indirect_block[block_index % entries_per_block];
        kfree(indirect_block);
        return result;
    }
    
    return 0;
}

uint32 ext2_alloc_block() {
    if (!ext2_mounted) {
        return 0;
    }
    
    for (uint32 group = 0; group < filesystem.num_block_groups; group++) {
        if (filesystem.block_groups[group].bg_free_blocks_count == 0) {
            continue;
        }
        
        uint8* bitmap = (uint8*)kmalloc(filesystem.block_size);
        if (!bitmap) {
            return 0;
        }
        
        uint32 bitmap_block = filesystem.block_groups[group].bg_block_bitmap;
        uint32 sectors_per_block = filesystem.block_size / SECTOR_SIZE;
        disk_read_sectors(filesystem.drive, filesystem.partition_offset + bitmap_block * sectors_per_block, sectors_per_block, bitmap);
        
        for (uint32 i = 0; i < filesystem.blocks_per_group; i++) {
            uint32 byte_idx = i / 8;
            uint32 bit_idx = i % 8;
            
            if (!(bitmap[byte_idx] & (1 << bit_idx))) {
                bitmap[byte_idx] |= (1 << bit_idx);
                
                disk_write_sectors(filesystem.drive, filesystem.partition_offset + bitmap_block * sectors_per_block, sectors_per_block, bitmap);
                
                filesystem.block_groups[group].bg_free_blocks_count--;
                filesystem.superblock.s_free_blocks_count--;
                
                kfree(bitmap);
                return group * filesystem.blocks_per_group + i + filesystem.superblock.s_first_data_block;
            }
        }
        
        kfree(bitmap);
    }
    
    return 0;
}

void ext2_free_block(uint32 block_num) {
    if (!ext2_mounted || block_num == 0) {
        return;
    }
    
    block_num -= filesystem.superblock.s_first_data_block;
    uint32 group = block_num / filesystem.blocks_per_group;
    uint32 index = block_num % filesystem.blocks_per_group;
    
    uint8* bitmap = (uint8*)kmalloc(filesystem.block_size);
    if (!bitmap) {
        return;
    }
    
    uint32 bitmap_block = filesystem.block_groups[group].bg_block_bitmap;
    uint32 sectors_per_block = filesystem.block_size / SECTOR_SIZE;
    disk_read_sectors(filesystem.drive, filesystem.partition_offset + bitmap_block * sectors_per_block, sectors_per_block, bitmap);
    
    uint32 byte_idx = index / 8;
    uint32 bit_idx = index % 8;
    bitmap[byte_idx] &= ~(1 << bit_idx);
    
    disk_write_sectors(filesystem.drive, filesystem.partition_offset + bitmap_block * sectors_per_block, sectors_per_block, bitmap);
    
    filesystem.block_groups[group].bg_free_blocks_count++;
    filesystem.superblock.s_free_blocks_count++;
    
    kfree(bitmap);
}

uint32 ext2_alloc_inode() {
    if (!ext2_mounted) {
        return 0;
    }
    
    for (uint32 group = 0; group < filesystem.num_block_groups; group++) {
        if (filesystem.block_groups[group].bg_free_inodes_count == 0) {
            continue;
        }
        
        uint8* bitmap = (uint8*)kmalloc(filesystem.block_size);
        if (!bitmap) {
            return 0;
        }
        
        uint32 bitmap_block = filesystem.block_groups[group].bg_inode_bitmap;
        uint32 sectors_per_block = filesystem.block_size / SECTOR_SIZE;
        disk_read_sectors(filesystem.drive, filesystem.partition_offset + bitmap_block * sectors_per_block, sectors_per_block, bitmap);
        
        for (uint32 i = 0; i < filesystem.inodes_per_group; i++) {
            uint32 byte_idx = i / 8;
            uint32 bit_idx = i % 8;
            
            if (!(bitmap[byte_idx] & (1 << bit_idx))) {
                bitmap[byte_idx] |= (1 << bit_idx);
                
                disk_write_sectors(filesystem.drive, filesystem.partition_offset + bitmap_block * sectors_per_block, sectors_per_block, bitmap);
                
                filesystem.block_groups[group].bg_free_inodes_count--;
                filesystem.superblock.s_free_inodes_count--;
                
                kfree(bitmap);
                return group * filesystem.inodes_per_group + i + 1;
            }
        }
        
        kfree(bitmap);
    }
    
    return 0;
}

void ext2_free_inode(uint32 inode_num) {
    if (!ext2_mounted || inode_num == 0) {
        return;
    }
    
    inode_num--;
    uint32 group = inode_num / filesystem.inodes_per_group;
    uint32 index = inode_num % filesystem.inodes_per_group;
    
    uint8* bitmap = (uint8*)kmalloc(filesystem.block_size);
    if (!bitmap) {
        return;
    }
    
    uint32 bitmap_block = filesystem.block_groups[group].bg_inode_bitmap;
    uint32 sectors_per_block = filesystem.block_size / SECTOR_SIZE;
    disk_read_sectors(filesystem.drive, filesystem.partition_offset + bitmap_block * sectors_per_block, sectors_per_block, bitmap);
    
    uint32 byte_idx = index / 8;
    uint32 bit_idx = index % 8;
    bitmap[byte_idx] &= ~(1 << bit_idx);
    
    disk_write_sectors(filesystem.drive, filesystem.partition_offset + bitmap_block * sectors_per_block, sectors_per_block, bitmap);
    
    filesystem.block_groups[group].bg_free_inodes_count++;
    filesystem.superblock.s_free_inodes_count++;
    
    kfree(bitmap);
}

ext2_dirent_t* ext2_find_dirent(ext2_inode_t* dir_inode, const char* name) {
    if (!dir_inode || !name) {
        return 0;
    }
    
    uint32 num_blocks = (dir_inode->i_size + filesystem.block_size - 1) / filesystem.block_size;
    uint8* block_buffer = (uint8*)kmalloc(filesystem.block_size);
    if (!block_buffer) {
        return 0;
    }
    
    for (uint32 i = 0; i < num_blocks; i++) {
        uint32 block_num = ext2_read_inode_block(dir_inode, i);
        if (block_num == 0) {
            continue;
        }
        
        uint32 sectors_per_block = filesystem.block_size / SECTOR_SIZE;
        disk_read_sectors(filesystem.drive, filesystem.partition_offset + block_num * sectors_per_block, sectors_per_block, block_buffer);
        
        uint32 offset = 0;
        while (offset < filesystem.block_size) {
            ext2_dirent_t* dirent = (ext2_dirent_t*)(block_buffer + offset);
            
            if (dirent->inode == 0 || dirent->rec_len == 0) {
                break;
            }
            
            char entry_name[256];
            memcpy(entry_name, dirent->name, dirent->name_len);
            entry_name[dirent->name_len] = '\0';
            
            if (strcmp(entry_name, name) == 0) {
                ext2_dirent_t* result = (ext2_dirent_t*)kmalloc(sizeof(ext2_dirent_t));
                if (result) {
                    memcpy(result, dirent, sizeof(ext2_dirent_t));
                }
                kfree(block_buffer);
                return result;
            }
            
            offset += dirent->rec_len;
        }
    }
    
    kfree(block_buffer);
    return 0;
}

static ext2_inode_t* ext2_traverse_path(const char* path, char* last_component) {
    if (!path || path[0] != '/') {
        return 0;
    }
    
    ext2_inode_t* current_inode = ext2_read_inode(EXT2_ROOT_INODE);
    if (!current_inode) {
        return 0;
    }
    
    if (strcmp(path, "/") == 0) {
        if (last_component) {
            last_component[0] = '\0';
        }
        return current_inode;
    }
    
    char path_copy[256];
    strcpy(path_copy, path + 1);
    
    char* token = path_copy;
    char* next_slash;
    
    while ((next_slash = strchr(token, '/')) != 0) {
        *next_slash = '\0';
        
        ext2_dirent_t* dirent = ext2_find_dirent(current_inode, token);
        if (!dirent) {
            kfree(current_inode);
            return 0;
        }
        
        uint32 inode_num = dirent->inode;
        kfree(dirent);
        kfree(current_inode);
        
        current_inode = ext2_read_inode(inode_num);
        if (!current_inode) {
            return 0;
        }
        
        token = next_slash + 1;
    }
    
    if (*token != '\0') {
        if (last_component) {
            strcpy(last_component, token);
        } else {
            ext2_dirent_t* dirent = ext2_find_dirent(current_inode, token);
            if (!dirent) {
                kfree(current_inode);
                return 0;
            }
            
            uint32 inode_num = dirent->inode;
            kfree(dirent);
            kfree(current_inode);
            
            current_inode = ext2_read_inode(inode_num);
        }
    } else {
        if (last_component) {
            last_component[0] = '\0';
        }
    }
    
    return current_inode;
}

ext2_file_t* ext2_open(const char* path, uint32 flags) {
    if (!ext2_mounted || !path) {
        return 0;
    }
    
    ext2_inode_t* inode = ext2_traverse_path(path, 0);
    if (!inode) {
        return 0;
    }
    
    ext2_file_t* file = (ext2_file_t*)kmalloc(sizeof(ext2_file_t));
    if (!file) {
        kfree(inode);
        return 0;
    }
    
    memcpy(&file->inode, inode, sizeof(ext2_inode_t));
    kfree(inode);
    
    file->position = 0;
    file->flags = flags;
    strcpy(file->name, path);
    
    return file;
}

int ext2_close(ext2_file_t* file) {
    if (!file) {
        return -1;
    }
    
    kfree(file);
    return 0;
}

int ext2_read(ext2_file_t* file, void* buffer, uint32 size) {
    if (!file || !buffer) {
        return -1;
    }
    
    if (file->position >= file->inode.i_size) {
        return 0;
    }
    
    if (file->position + size > file->inode.i_size) {
        size = file->inode.i_size - file->position;
    }
    
    uint32 bytes_read = 0;
    uint8* byte_buffer = (uint8*)buffer;
    uint8* block_buffer = (uint8*)kmalloc(filesystem.block_size);
    
    if (!block_buffer) {
        return -1;
    }
    
    while (bytes_read < size) {
        uint32 block_index = file->position / filesystem.block_size;
        uint32 block_offset = file->position % filesystem.block_size;
        uint32 bytes_to_read = filesystem.block_size - block_offset;
        
        if (bytes_to_read > size - bytes_read) {
            bytes_to_read = size - bytes_read;
        }
        
        uint32 block_num = ext2_read_inode_block(&file->inode, block_index);
        if (block_num == 0) {
            kfree(block_buffer);
            return bytes_read;
        }
        
        uint32 sectors_per_block = filesystem.block_size / SECTOR_SIZE;
        disk_read_sectors(filesystem.drive, filesystem.partition_offset + block_num * sectors_per_block, sectors_per_block, block_buffer);
        
        memcpy(byte_buffer + bytes_read, block_buffer + block_offset, bytes_to_read);
        
        bytes_read += bytes_to_read;
        file->position += bytes_to_read;
    }
    
    kfree(block_buffer);
    return bytes_read;
}

int ext2_readdir(const char* path, ext2_dirent_t* entries, int max_entries) {
    if (!ext2_mounted || !path || !entries) {
        return -1;
    }
    
    ext2_inode_t* dir_inode = ext2_traverse_path(path, 0);
    if (!dir_inode) {
        return -1;
    }
    
    if ((dir_inode->i_mode & EXT2_S_IFDIR) != EXT2_S_IFDIR) {
        kfree(dir_inode);
        return -1;
    }
    
    uint32 num_blocks = (dir_inode->i_size + filesystem.block_size - 1) / filesystem.block_size;
    uint8* block_buffer = (uint8*)kmalloc(filesystem.block_size);
    if (!block_buffer) {
        kfree(dir_inode);
        return -1;
    }
    
    int count = 0;
    
    for (uint32 i = 0; i < num_blocks && count < max_entries; i++) {
        uint32 block_num = ext2_read_inode_block(dir_inode, i);
        if (block_num == 0) {
            continue;
        }
        
        uint32 sectors_per_block = filesystem.block_size / SECTOR_SIZE;
        disk_read_sectors(filesystem.drive, filesystem.partition_offset + block_num * sectors_per_block, sectors_per_block, block_buffer);
        
        uint32 offset = 0;
        while (offset < filesystem.block_size && count < max_entries) {
            ext2_dirent_t* dirent = (ext2_dirent_t*)(block_buffer + offset);
            
            if (dirent->inode == 0 || dirent->rec_len == 0) {
                break;
            }
            
            memcpy(&entries[count], dirent, sizeof(ext2_dirent_t));
            count++;
            
            offset += dirent->rec_len;
        }
    }
    
    kfree(block_buffer);
    kfree(dir_inode);
    
    return count;
}

void ext2_list_dir(const char* path) {
    ext2_dirent_t entries[64];
    int count = ext2_readdir(path, entries, 64);
    
    if (count < 0) {
        printf("Error reading directory\n");
        return;
    }
    
    printf("Directory listing for: ");
    printf(path);
    printf("\n");
    
    for (int i = 0; i < count; i++) {
        char name[256];
        memcpy(name, entries[i].name, entries[i].name_len);
        name[entries[i].name_len] = '\0';
        
        printf("  ");
        
        if (entries[i].file_type == EXT2_FT_DIR) {
            set_screen_color(0x0B, 0x00);
            printf("[DIR]  ");
        } else {
            set_screen_color(0x0F, 0x00);
            printf("[FILE] ");
        }
        
        printf(name);
        printf("\n");
        set_screen_color(0x0F, 0x00);
    }
}

ext2_fs_t* ext2_get_fs() {
    return &filesystem;
}

void ext2_print_superblock() {
    if (!ext2_mounted) {
        printf("Filesystem not mounted\n");
        return;
    }
    
    printf("EXT2 Filesystem Information:\n");
    printf("  Block size: ");
    char str[20];
    int_to_ascii(filesystem.block_size, str);
    printf(str);
    printf(" bytes\n");
    
    printf("  Total blocks: ");
    int_to_ascii(filesystem.superblock.s_blocks_count, str);
    printf(str);
    printf("\n");
    
    printf("  Free blocks: ");
    int_to_ascii(filesystem.superblock.s_free_blocks_count, str);
    printf(str);
    printf("\n");
    
    printf("  Total inodes: ");
    int_to_ascii(filesystem.superblock.s_inodes_count, str);
    printf(str);
    printf("\n");
    
    printf("  Free inodes: ");
    int_to_ascii(filesystem.superblock.s_free_inodes_count, str);
    printf(str);
    printf("\n");
    
    printf("  Block groups: ");
    int_to_ascii(filesystem.num_block_groups, str);
    printf(str);
    printf("\n");
}

int ext2_stat(const char* path, ext2_inode_t* inode) {
    if (!ext2_mounted || !path || !inode) {
        return -1;
    }
    
    ext2_inode_t* file_inode = ext2_traverse_path(path, 0);
    if (!file_inode) {
        return -1;
    }
    
    memcpy(inode, file_inode, sizeof(ext2_inode_t));
    kfree(file_inode);
    
    return 0;
}
