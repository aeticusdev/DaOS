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

#ifndef DISK_H
#define DISK_H

#include "types.h"

#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170
#define ATA_PRIMARY_DCR_AS 0x3F6
#define ATA_SECONDARY_DCR_AS 0x376
#define ATA_PRIMARY_IRQ 14
#define ATA_SECONDARY_IRQ 15

#define ATA_REG_DATA 0
#define ATA_REG_ERROR 1
#define ATA_REG_FEATURES 1
#define ATA_REG_SECCOUNT 2
#define ATA_REG_LBA0 3
#define ATA_REG_LBA1 4
#define ATA_REG_LBA2 5
#define ATA_REG_HDDEVSEL 6
#define ATA_REG_COMMAND 7
#define ATA_REG_STATUS 7

#define ATA_SR_BSY 0x80
#define ATA_SR_DRDY 0x40
#define ATA_SR_DF 0x20
#define ATA_SR_DSC 0x10
#define ATA_SR_DRQ 0x08
#define ATA_SR_CORR 0x04
#define ATA_SR_IDX 0x02
#define ATA_SR_ERR 0x01

#define ATA_CMD_READ_PIO 0x20
#define ATA_CMD_READ_PIO_EXT 0x24
#define ATA_CMD_READ_DMA 0xC8
#define ATA_CMD_READ_DMA_EXT 0x25
#define ATA_CMD_WRITE_PIO 0x30
#define ATA_CMD_WRITE_PIO_EXT 0x34
#define ATA_CMD_WRITE_DMA 0xCA
#define ATA_CMD_WRITE_DMA_EXT 0x35
#define ATA_CMD_CACHE_FLUSH 0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET 0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY 0xEC

#define ATA_IDENT_DEVICETYPE 0
#define ATA_IDENT_CYLINDERS 2
#define ATA_IDENT_HEADS 6
#define ATA_IDENT_SECTORS 12
#define ATA_IDENT_SERIAL 20
#define ATA_IDENT_MODEL 54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID 106
#define ATA_IDENT_MAX_LBA 120
#define ATA_IDENT_COMMANDSETS 164
#define ATA_IDENT_MAX_LBA_EXT 200

#define ATA_MASTER 0x00
#define ATA_SLAVE 0x01

#define SECTOR_SIZE 512

typedef struct ata_device {
    uint16 base;
    uint16 ctrl;
    uint8 slave;
    uint16 type;
    uint16 signature;
    uint16 capabilities;
    uint32 command_sets;
    uint32 size;
    char model[41];
} ata_device_t;

void init_disk();
int disk_detect(uint8 bus, uint8 drive);
void disk_identify(uint8 bus, uint8 drive);

int disk_read_sector(uint8 drive, uint32 lba, uint8* buffer);
int disk_write_sector(uint8 drive, uint32 lba, uint8* buffer);
int disk_read_sectors(uint8 drive, uint32 lba, uint8 count, uint8* buffer);
int disk_write_sectors(uint8 drive, uint32 lba, uint8 count, uint8* buffer);

void disk_wait_ready(uint16 base);
void disk_wait_drq(uint16 base);
uint8 disk_get_status(uint16 base);
void disk_select_drive(uint16 base, uint8 slave);

ata_device_t* disk_get_device(uint8 drive);
void disk_print_info();

#endif
