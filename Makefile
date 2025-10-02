COMPILER = gcc
LINKER = ld
ASSEMBLER = nasm

CFLAGS = -m32 -c -ffreestanding -fno-stack-protector
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T src/link.ld

EMULATOR = qemu-system-i386
EMULATOR_FLAGS = -kernel

OBJS = obj/kasm.o obj/kc.o obj/idt.o obj/isr.o obj/irq.o obj/irqasm.o obj/kb.o obj/screen.o obj/string.o obj/system.o obj/util.o obj/shell.o obj/snake.o obj/memory.o obj/fs.o obj/timer.o obj/process.o obj/syscall.o obj/hal.o
OUTPUT = tmp/boot/kernel.bin

run: all
	$(EMULATOR) $(EMULATOR_FLAGS) $(OUTPUT)

all: $(OBJS)
	mkdir -p tmp/boot/
	$(LINKER) $(LDFLAGS) -o $(OUTPUT) $(OBJS)

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

build: all
	grub-mkrescue -o cavOS.iso tmp/

clean:
	rm -f obj/*.o
	rm -rf tmp/

.PHONY: run all build clean