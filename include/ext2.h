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

#ifndef EXT2_H
#define EXT2_H

#include "types.h"
#include "disk.h"

#define EXT2_SIGNATURE 0xEF53
#define EXT2_SUPER_MAGIC 0xEF53

#define EXT2_DIRECT_BLOCKS 12
#define EXT2_INDIRECT_BLOCKS 1
#define EXT2_DOUBLE_INDIRECT_BLOCKS 1
#define EXT2_TRIPLE_INDIRECT_BLOCKS 1
#define EXT2_TOTAL_BLOCKS 15

#define EXT2_ROOT_INODE 2

#define EXT2_S_IFSOCK 0xC000
#define EXT2_S_IFLNK 0xA000
#define EXT2_S_IFREG 0x8000
#define EXT2_S_IFBLK 0x6000
#define EXT2_S_IFDIR 0x4000
#define EXT2_S_IFCHR 0x2000
#define EXT2_S_IFIFO 0x1000

#define EXT2_FT_UNKNOWN 0
#define EXT2_FT_REG_FILE 1
#define EXT2_FT_DIR 2
#define EXT2_FT_CHRDEV 3
#define EXT2_FT_BLKDEV 4
#define EXT2_FT_FIFO 5
#define EXT2_FT_SOCK 6
#define EXT2_FT_SYMLINK 7

#define EXT2_NAME_LEN 255

typedef struct ext2_superblock {
    uint32 s_inodes_count;
    uint32 s_blocks_count;
    uint32 s_r_blocks_count;
    uint32 s_free_blocks_count;
    uint32 s_free_inodes_count;
    uint32 s_first_data_block;
    uint32 s_log_block_size;
    uint32 s_log_frag_size;
    uint32 s_blocks_per_group;
    uint32 s_frags_per_group;
    uint32 s_inodes_per_group;
    uint32 s_mtime;
    uint32 s_wtime;
    uint16 s_mnt_count;
    uint16 s_max_mnt_count;
    uint16 s_magic;
    uint16 s_state;
    uint16 s_errors;
    uint16 s_minor_rev_level;
    uint32 s_lastcheck;
    uint32 s_checkinterval;
    uint32 s_creator_os;
    uint32 s_rev_level;
    uint16 s_def_resuid;
    uint16 s_def_resgid;
    uint32 s_first_ino;
    uint16 s_inode_size;
    uint16 s_block_group_nr;
    uint32 s_feature_compat;
    uint32 s_feature_incompat;
    uint32 s_feature_ro_compat;
    uint8 s_uuid[16];
    char s_volume_name[16];
    char s_last_mounted[64];
    uint32 s_algorithm_usage_bitmap;
    uint8 s_prealloc_blocks;
    uint8 s_prealloc_dir_blocks;
    uint16 s_padding1;
    uint8 s_journal_uuid[16];
    uint32 s_journal_inum;
    uint32 s_journal_dev;
    uint32 s_last_orphan;
    uint32 s_hash_seed[4];
    uint8 s_def_hash_version;
    uint8 s_reserved_char_pad;
    uint16 s_reserved_word_pad;
    uint32 s_default_mount_opts;
    uint32 s_first_meta_bg;
    uint8 s_reserved[760];
} __attribute__((packed)) ext2_superblock_t;

typedef struct ext2_block_group_descriptor {
    uint32 bg_block_bitmap;
    uint32 bg_inode_bitmap;
    uint32 bg_inode_table;
    uint16 bg_free_blocks_count;
    uint16 bg_free_inodes_count;
    uint16 bg_used_dirs_count;
    uint16 bg_pad;
    uint8 bg_reserved[12];
} __attribute__((packed)) ext2_bgd_t;

typedef struct ext2_inode {
    uint16 i_mode;
    uint16 i_uid;
    uint32 i_size;
    uint32 i_atime;
    uint32 i_ctime;
    uint32 i_mtime;
    uint32 i_dtime;
    uint16 i_gid;
    uint16 i_links_count;
    uint32 i_blocks;
    uint32 i_flags;
    uint32 i_osd1;
    uint32 i_block[EXT2_TOTAL_BLOCKS];
    uint32 i_generation;
    uint32 i_file_acl;
    uint32 i_dir_acl;
    uint32 i_faddr;
    uint8 i_osd2[12];
} __attribute__((packed)) ext2_inode_t;

typedef struct ext2_dir_entry {
    uint32 inode;
    uint16 rec_len;
    uint8 name_len;
    uint8 file_type;
    char name[EXT2_NAME_LEN];
} __attribute__((packed)) ext2_dirent_t;

typedef struct ext2_file {
    ext2_inode_t inode;
    uint32 inode_num;
    uint32 position;
    uint32 flags;
    char name[256];
} ext2_file_t;

typedef struct ext2_filesystem {
    uint8 drive;
    uint32 partition_offset;
    ext2_superblock_t superblock;
    ext2_bgd_t* block_groups;
    uint32 block_size;
    uint32 num_block_groups;
    uint32 inodes_per_group;
    uint32 blocks_per_group;
} ext2_fs_t;

void ext2_init(uint8 drive, uint32 partition_offset);
int ext2_mount();
void ext2_unmount();

ext2_inode_t* ext2_read_inode(uint32 inode_num);
int ext2_write_inode(uint32 inode_num, ext2_inode_t* inode);

uint32 ext2_read_inode_block(ext2_inode_t* inode, uint32 block_index);
int ext2_write_inode_block(ext2_inode_t* inode, uint32 block_index, uint32 block_num);

uint32 ext2_alloc_block();
void ext2_free_block(uint32 block_num);
uint32 ext2_alloc_inode();
void ext2_free_inode(uint32 inode_num);

ext2_dirent_t* ext2_find_dirent(ext2_inode_t* dir_inode, const char* name);
int ext2_create_dirent(ext2_inode_t* dir_inode, uint32 dir_inode_num, const char* name, uint32 inode_num, uint8 file_type);
int ext2_remove_dirent(ext2_inode_t* dir_inode, const char* name);

ext2_file_t* ext2_open(const char* path, uint32 flags);
int ext2_close(ext2_file_t* file);
int ext2_read(ext2_file_t* file, void* buffer, uint32 size);
int ext2_write(ext2_file_t* file, const void* buffer, uint32 size);
int ext2_seek(ext2_file_t* file, uint32 offset, int whence);

int ext2_create(const char* path, uint16 mode);
int ext2_mkdir(const char* path, uint16 mode);
int ext2_unlink(const char* path);
int ext2_rmdir(const char* path);

int ext2_readdir(const char* path, ext2_dirent_t* entries, int max_entries);
int ext2_stat(const char* path, ext2_inode_t* inode);

ext2_fs_t* ext2_get_fs();
void ext2_print_superblock();
void ext2_list_dir(const char* path);

#endif
