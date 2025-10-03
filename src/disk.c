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

#include "../include/disk.h"
#include "../include/system.h"
#include "../include/screen.h"
#include "../include/string.h"
#include "../include/util.h"

static ata_device_t ata_devices[4];
static int num_devices = 0;

void disk_wait_ready(uint16 base) {
    while ((inportb(base + ATA_REG_STATUS) & ATA_SR_BSY) != 0);
}

void disk_wait_drq(uint16 base) {
    while ((inportb(base + ATA_REG_STATUS) & ATA_SR_DRQ) == 0);
}

uint8 disk_get_status(uint16 base) {
    return inportb(base + ATA_REG_STATUS);
}

void disk_select_drive(uint16 base, uint8 slave) {
    if (slave == ATA_MASTER) {
        outportb(base + ATA_REG_HDDEVSEL, 0xA0);
    } else {
        outportb(base + ATA_REG_HDDEVSEL, 0xB0);
    }
}

void disk_identify(uint8 bus, uint8 drive) {
    uint16 base = (bus == 0) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
    uint8 slave = drive;
    
    disk_select_drive(base, slave);
    
    outportb(base + ATA_REG_SECCOUNT, 0);
    outportb(base + ATA_REG_LBA0, 0);
    outportb(base + ATA_REG_LBA1, 0);
    outportb(base + ATA_REG_LBA2, 0);
    outportb(base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
    
    uint8 status = disk_get_status(base);
    if (status == 0) {
        return;
    }
    
    disk_wait_ready(base);
    
    uint8 lba1 = inportb(base + ATA_REG_LBA1);
    uint8 lba2 = inportb(base + ATA_REG_LBA2);
    
    if (lba1 != 0 || lba2 != 0) {
        return;
    }
    
    disk_wait_drq(base);
    
    uint16 identify_data[256];
    for (int i = 0; i < 256; i++) {
        identify_data[i] = inportw(base + ATA_REG_DATA);
    }
    
    int idx = num_devices;
    ata_devices[idx].base = base;
    ata_devices[idx].ctrl = (bus == 0) ? ATA_PRIMARY_DCR_AS : ATA_SECONDARY_DCR_AS;
    ata_devices[idx].slave = slave;
    ata_devices[idx].type = 0;
    ata_devices[idx].signature = identify_data[ATA_IDENT_DEVICETYPE];
    ata_devices[idx].capabilities = identify_data[ATA_IDENT_CAPABILITIES];
    ata_devices[idx].command_sets = *((uint32*)(identify_data + ATA_IDENT_COMMANDSETS));
    
    if (ata_devices[idx].command_sets & (1 << 26)) {
        ata_devices[idx].size = *((uint32*)(identify_data + ATA_IDENT_MAX_LBA_EXT));
    } else {
        ata_devices[idx].size = *((uint32*)(identify_data + ATA_IDENT_MAX_LBA));
    }
    
    for (int i = 0; i < 40; i += 2) {
        ata_devices[idx].model[i] = identify_data[ATA_IDENT_MODEL + i/2] >> 8;
        ata_devices[idx].model[i + 1] = identify_data[ATA_IDENT_MODEL + i/2] & 0xFF;
    }
    ata_devices[idx].model[40] = '\0';
    
    for (int i = 39; i >= 0 && ata_devices[idx].model[i] == ' '; i--) {
        ata_devices[idx].model[i] = '\0';
    }
    
    num_devices++;
}

int disk_detect(uint8 bus, uint8 drive) {
    uint16 base = (bus == 0) ? ATA_PRIMARY_IO : ATA_SECONDARY_IO;
    
    disk_select_drive(base, drive);
    
    for (int i = 0; i < 1000; i++) {
        __asm__ __volatile__("nop");
    }
    
    uint8 status = disk_get_status(base);
    
    if (status == 0xFF || status == 0) {
        return 0;
    }
    
    return 1;
}

void init_disk() {
    num_devices = 0;
    
    printf("  Detecting ATA drives...\n");
    
    if (disk_detect(0, ATA_MASTER)) {
        printf("    Primary Master detected\n");
        disk_identify(0, ATA_MASTER);
    } else {
        printf("    Primary Master not found\n");
    }
    
    if (disk_detect(0, ATA_SLAVE)) {
        printf("    Primary Slave detected\n");
        disk_identify(0, ATA_SLAVE);
    } else {
        printf("    Primary Slave not found\n");
    }
    
    if (disk_detect(1, ATA_MASTER)) {
        printf("    Secondary Master detected\n");
        disk_identify(1, ATA_MASTER);
    } else {
        printf("    Secondary Master not found\n");
    }
    
    if (disk_detect(1, ATA_SLAVE)) {
        printf("    Secondary Slave detected\n");
        disk_identify(1, ATA_SLAVE);
    } else {
        printf("    Secondary Slave not found\n");
    }
    
    printf("  Total drives detected: ");
    char num_str[10];
    int_to_ascii(num_devices, num_str);
    printf(num_str);
    printf("\n");
}

int disk_read_sector(uint8 drive, uint32 lba, uint8* buffer) {
    if (drive >= num_devices) {
        return -1;
    }
    
    ata_device_t* dev = &ata_devices[drive];
    
    disk_wait_ready(dev->base);
    
    disk_select_drive(dev->base, dev->slave);
    outportb(dev->base + ATA_REG_HDDEVSEL, 0xE0 | (dev->slave << 4) | ((lba >> 24) & 0x0F));
    
    outportb(dev->base + ATA_REG_FEATURES, 0);
    outportb(dev->base + ATA_REG_SECCOUNT, 1);
    outportb(dev->base + ATA_REG_LBA0, (uint8)lba);
    outportb(dev->base + ATA_REG_LBA1, (uint8)(lba >> 8));
    outportb(dev->base + ATA_REG_LBA2, (uint8)(lba >> 16));
    outportb(dev->base + ATA_REG_COMMAND, ATA_CMD_READ_PIO);
    
    disk_wait_drq(dev->base);
    
    uint16* word_buffer = (uint16*)buffer;
    for (int i = 0; i < 256; i++) {
        word_buffer[i] = inportw(dev->base + ATA_REG_DATA);
    }
    
    disk_wait_ready(dev->base);
    
    return 0;
}

int disk_write_sector(uint8 drive, uint32 lba, uint8* buffer) {
    if (drive >= num_devices) {
        return -1;
    }
    
    ata_device_t* dev = &ata_devices[drive];
    
    disk_wait_ready(dev->base);
    
    disk_select_drive(dev->base, dev->slave);
    outportb(dev->base + ATA_REG_HDDEVSEL, 0xE0 | (dev->slave << 4) | ((lba >> 24) & 0x0F));
    
    outportb(dev->base + ATA_REG_FEATURES, 0);
    outportb(dev->base + ATA_REG_SECCOUNT, 1);
    outportb(dev->base + ATA_REG_LBA0, (uint8)lba);
    outportb(dev->base + ATA_REG_LBA1, (uint8)(lba >> 8));
    outportb(dev->base + ATA_REG_LBA2, (uint8)(lba >> 16));
    outportb(dev->base + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);
    
    disk_wait_drq(dev->base);
    
    uint16* word_buffer = (uint16*)buffer;
    for (int i = 0; i < 256; i++) {
        outportw(dev->base + ATA_REG_DATA, word_buffer[i]);
    }
    
    outportb(dev->base + ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);
    disk_wait_ready(dev->base);
    
    return 0;
}

int disk_read_sectors(uint8 drive, uint32 lba, uint8 count, uint8* buffer) {
    for (uint8 i = 0; i < count; i++) {
        int result = disk_read_sector(drive, lba + i, buffer + (i * SECTOR_SIZE));
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

int disk_write_sectors(uint8 drive, uint32 lba, uint8 count, uint8* buffer) {
    for (uint8 i = 0; i < count; i++) {
        int result = disk_write_sector(drive, lba + i, buffer + (i * SECTOR_SIZE));
        if (result != 0) {
            return result;
        }
    }
    return 0;
}

ata_device_t* disk_get_device(uint8 drive) {
    if (drive >= num_devices) {
        return 0;
    }
    return &ata_devices[drive];
}

void disk_print_info() {
    printf("Disk Devices:\n");
    for (int i = 0; i < num_devices; i++) {
        printf("Drive ");
        char drive_str[10];
        int_to_ascii(i, drive_str);
        printf(drive_str);
        printf(": ");
        printf(ata_devices[i].model);
        printf(" (");
        char size_str[20];
        int_to_ascii(ata_devices[i].size / 2048, size_str);
        printf(size_str);
        printf(" MB)\n");
    }
}
