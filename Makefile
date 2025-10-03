COMPILER = gcc
LINKER = ld
ASSEMBLER = nasm

CFLAGS = -m64 -c -ffreestanding -fno-stack-protector -mcmodel=large -mno-red-zone -mno-mmx -mno-sse -mno-sse2
ASFLAGS = -f elf64
LDFLAGS = -m elf_x86_64 -T src/link.ld

EMULATOR = qemu-system-x86_64

OBJS = obj/kasm.o obj/kc.o obj/idt.o obj/isr.o obj/irq.o obj/irqasm.o obj/kb.o obj/screen.o obj/string.o obj/system.o obj/util.o obj/shell.o obj/snake.o obj/memory.o obj/fs.o obj/timer.o obj/process.o obj/syscall.o obj/hal.o obj/pmm.o obj/paging.o obj/dma.o obj/disk.o obj/ext2.o
OUTPUT = tmp/boot/kernel.bin
ISO = daos.iso
DISK_IMG = disk.img

run: iso
	$(EMULATOR) -cdrom $(ISO) -hda $(DISK_IMG) -m 512M

all: $(OBJS)
	mkdir -p tmp/boot/
	$(LINKER) $(LDFLAGS) -o $(OUTPUT) $(OBJS)

iso: all
	mkdir -p tmp/boot/grub
	cp grub.cfg tmp/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) tmp/

obj/kasm.o: src/kernel.asm
	mkdir -p obj/
	$(ASSEMBLER) $(ASFLAGS) -o obj/kasm.o src/kernel.asm

obj/kc.o: src/kernel.c
	$(COMPILER) $(CFLAGS) src/kernel.c -o obj/kc.o

obj/idt.o: src/idt.c
	$(COMPILER) $(CFLAGS) src/idt.c -o obj/idt.o

obj/kb.o: src/kb.c
	$(COMPILER) $(CFLAGS) src/kb.c -o obj/kb.o

obj/isr.o: src/isr.c
	$(COMPILER) $(CFLAGS) src/isr.c -o obj/isr.o

obj/irq.o: src/irq.c
	$(COMPILER) $(CFLAGS) src/irq.c -o obj/irq.o

obj/irqasm.o: src/irq.asm
	$(ASSEMBLER) $(ASFLAGS) -o obj/irqasm.o src/irq.asm

obj/screen.o: src/screen.c
	$(COMPILER) $(CFLAGS) src/screen.c -o obj/screen.o

obj/string.o: src/string.c
	$(COMPILER) $(CFLAGS) src/string.c -o obj/string.o

obj/system.o: src/system.c
	$(COMPILER) $(CFLAGS) src/system.c -o obj/system.o

obj/util.o: src/util.c
	$(COMPILER) $(CFLAGS) src/util.c -o obj/util.o

obj/shell.o: src/shell.c
	$(COMPILER) $(CFLAGS) src/shell.c -o obj/shell.o

obj/snake.o: src/snake.c
	$(COMPILER) $(CFLAGS) src/snake.c -o obj/snake.o

obj/memory.o: src/memory.c
	$(COMPILER) $(CFLAGS) src/memory.c -o obj/memory.o

obj/fs.o: src/fs.c
	$(COMPILER) $(CFLAGS) src/fs.c -o obj/fs.o

obj/timer.o: src/timer.c
	$(COMPILER) $(CFLAGS) src/timer.c -o obj/timer.o

obj/process.o: src/process.c
	$(COMPILER) $(CFLAGS) src/process.c -o obj/process.o

obj/syscall.o: src/syscall.c
	$(COMPILER) $(CFLAGS) src/syscall.c -o obj/syscall.o

obj/hal.o: src/hal.c
	$(COMPILER) $(CFLAGS) src/hal.c -o obj/hal.o

obj/pmm.o: src/pmm.c
	$(COMPILER) $(CFLAGS) src/pmm.c -o obj/pmm.o

obj/paging.o: src/paging.c
	$(COMPILER) $(CFLAGS) src/paging.c -o obj/paging.o

obj/dma.o: src/dma.c
	$(COMPILER) $(CFLAGS) src/dma.c -o obj/dma.o

obj/disk.o: src/disk.c
	$(COMPILER) $(CFLAGS) src/disk.c -o obj/disk.o

obj/ext2.o: src/ext2.c
	$(COMPILER) $(CFLAGS) src/ext2.c -o obj/ext2.o

disk-image:
	dd if=/dev/zero of=$(DISK_IMG) bs=1M count=2048
	mkfs.ext2 -F $(DISK_IMG)
	mkdir -p mnt
	sudo mount -o loop $(DISK_IMG) mnt
	sudo mkdir -p mnt/boot mnt/home mnt/etc
	echo "Welcome to DaOS!" | sudo tee mnt/boot/readme.txt
	echo "Test file" | sudo tee mnt/home/test.txt
	sudo umount mnt
	rmdir mnt

clean:
	rm -f obj/*.o
	rm -rf tmp/
	rm -f $(ISO)

.PHONY: run all iso clean disk-image