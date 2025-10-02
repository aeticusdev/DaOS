#include "../include/idt.h"
#include "../include/util.h"
#include "../include/isr.h"
#include "../include/screen.h"

string exception_messages[32];

void isr_install() {
    set_idt_gate(0, (uint32)isr0);
    set_idt_gate(1, (uint32)isr1);
    set_idt_gate(2, (uint32)isr2);
    set_idt_gate(3, (uint32)isr3);
    set_idt_gate(4, (uint32)isr4);
    set_idt_gate(5, (uint32)isr5);
    set_idt_gate(6, (uint32)isr6);
    set_idt_gate(7, (uint32)isr7);
    set_idt_gate(8, (uint32)isr8);
    set_idt_gate(9, (uint32)isr9);
    set_idt_gate(10, (uint32)isr10);
    set_idt_gate(11, (uint32)isr11);
    set_idt_gate(12, (uint32)isr12);
    set_idt_gate(13, (uint32)isr13);
    set_idt_gate(14, (uint32)isr14);
    set_idt_gate(15, (uint32)isr15);
    set_idt_gate(16, (uint32)isr16);
    set_idt_gate(17, (uint32)isr17);
    set_idt_gate(18, (uint32)isr18);
    set_idt_gate(19, (uint32)isr19);
    set_idt_gate(20, (uint32)isr20);
    set_idt_gate(21, (uint32)isr21);
    set_idt_gate(22, (uint32)isr22);
    set_idt_gate(23, (uint32)isr23);
    set_idt_gate(24, (uint32)isr24);
    set_idt_gate(25, (uint32)isr25);
    set_idt_gate(26, (uint32)isr26);
    set_idt_gate(27, (uint32)isr27);
    set_idt_gate(28, (uint32)isr28);
    set_idt_gate(29, (uint32)isr29);
    set_idt_gate(30, (uint32)isr30);
    set_idt_gate(31, (uint32)isr31);

    exception_messages[0] = "Division By Zero";
    exception_messages[1] = "Debug";
    exception_messages[2] = "Non Maskable Interrupt";
    exception_messages[3] = "Breakpoint";
    exception_messages[4] = "Into Detected Overflow";
    exception_messages[5] = "Out of Bounds";
    exception_messages[6] = "Invalid Opcode";
    exception_messages[7] = "No Coprocessor";
    exception_messages[8] = "Double Fault";
    exception_messages[9] = "Coprocessor Segment Overrun";
    exception_messages[10] = "Bad TSS";
    exception_messages[11] = "Segment Not Present";
    exception_messages[12] = "Stack Fault";
    exception_messages[13] = "General Protection Fault";
    exception_messages[14] = "Page Fault";
    exception_messages[15] = "Unknown Interrupt";
    exception_messages[16] = "Coprocessor Fault";
    exception_messages[17] = "Alignment Check";
    exception_messages[18] = "Machine Check";
    exception_messages[19] = "Reserved";
    exception_messages[20] = "Reserved";
    exception_messages[21] = "Reserved";
    exception_messages[22] = "Reserved";
    exception_messages[23] = "Reserved";
    exception_messages[24] = "Reserved";
    exception_messages[25] = "Reserved";
    exception_messages[26] = "Reserved";
    exception_messages[27] = "Reserved";
    exception_messages[28] = "Reserved";
    exception_messages[29] = "Reserved";
    exception_messages[30] = "Reserved";
    exception_messages[31] = "Reserved";
    set_idt();
}

void isr0() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Division By Zero\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr1() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Debug\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr2() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Non Maskable Interrupt\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr3() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Breakpoint\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr4() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Into Detected Overflow\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr5() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Out of Bounds\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr6() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Invalid Opcode\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr7() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: No Coprocessor\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr8() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Double Fault\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr9() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Coprocessor Segment Overrun\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr10() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Bad TSS\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr11() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Segment Not Present\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr12() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Stack Fault\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr13() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: General Protection Fault\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr14() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Page Fault\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr15() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Unknown Interrupt\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr16() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Coprocessor Fault\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr17() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Alignment Check\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr18() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Machine Check\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr19() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Reserved\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr20() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Reserved\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr21() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Reserved\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr22() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Reserved\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr23() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Reserved\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr24() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Reserved\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr25() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Reserved\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr26() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Reserved\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr27() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Reserved\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr28() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Reserved\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr29() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Reserved\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr30() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Reserved\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
void isr31() {
    set_screen_color(0x0C, 0x00);
    printf("Exception: Reserved\n");
    set_screen_color(0x07, 0x00);
    while (1);
}
