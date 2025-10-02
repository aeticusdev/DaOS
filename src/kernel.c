#include "../include/kb.h"
#include "../include/shell.h"
#include "../include/util.h"
#include "../include/screen.h"
#include "../include/isr.h"
#include "../include/idt.h"

void kmain() {
    clearScreen();
    set_screen_color(0x0F, 0x00);
    printf("DaOS Kernel Initialized.\n");
    isr_install();
    printf("Interrupt Service Routines Installed.\n");
    printf("Launching Shell...\n\n");
    launch_shell(0);
    while (1);
}