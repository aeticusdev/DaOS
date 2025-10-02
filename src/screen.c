#include "../include/screen.h"
int cursorX = 0, cursorY = 0;
const uint8 sw = 80, sh = 25;
uint8 sd = 2;

void clearLine(uint8 from, uint8 to) {
    for (int i = from; i < to; i++) {
        for (int j = 0; j < sw; j++) {
            uint16 *where = (uint16 *)0xB8000 + (i * sw + j);
            *where = (0x0F << 8) | ' ';
        }
    }
}
void updateCursor() {
    uint16 pos = cursorY * sw + cursorX;
    outportb(0x3D4, 0x0F);
    outportb(0x3D5, (uint8)(pos & 0xFF));
    outportb(0x3D4, 0x0E);
    outportb(0x3D5, (uint8)((pos >> 8) & 0xFF));
}

void clearScreen() {
    clearLine(0, sh);
    cursorX = 0;
    cursorY = 0;
    updateCursor();
}

void scrollUp(uint8 lineNumber) {
    if (cursorY >= sh) {
        for (int i = 0; i < sh - 1; i++) {
            for (int j = 0; j < sw; j++) {
                uint16 *src = (uint16 *)0xB8000 + ((i + 1) * sw + j);
                uint16 *dest = (uint16 *)0xB8000 + (i * sw + j);
                *dest = *src;
            }
        }
        clearLine(sh - 1, sh);
        cursorY = sh - 1;
        updateCursor();
    }
}

void newLineCheck() {
    if (cursorX >= sw) {
        cursorX = 0;
        cursorY++;
    }
    scrollUp(1);
    updateCursor();
}

void printfch(char c) {
    if (c == '\n') {
        cursorX = 0;
        cursorY++;
        newLineCheck();
        updateCursor();
        return;
    }
    uint16 *where = (uint16 *)0xB8000 + (cursorY * sw + cursorX);
    *where = (sd << 8) | c;
    cursorX++;
    newLineCheck();
    updateCursor();
}

void printf(string ch) {
    for (int i = 0; ch[i] != '\0'; i++) {
        printfch(ch[i]);
    }
}

void set_screen_color_from_color_code(int color_code) {
    sd = color_code;
}

void set_screen_color(int text_color, int bg_color) {
    sd = (bg_color << 4) | (text_color & 0x0F);
}

void printf_colored(string ch, int text_color, int bg_color) {
    int prev_color = sd;
    set_screen_color(text_color, bg_color);
    printf(ch);
    sd = prev_color;
}