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

#include "../include/kb.h"

// US QWERTY keyboard scan code to ASCII mapping
const char scancode_to_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' '
};

string readStr(){
    string str = (string)malloc(100);
    int index = 0;
    while(1){
        char c = readKeys();
        if(c == '\b'){
            if(index > 0){
                index--;
                cursorX--;
                printfch(' ');
                cursorX--;
                updateCursor();
            }
        }
        else if(c == '\n'){
            str[index] = '\0';
            printfch('\n');
            return str;
        }
        else{
            str[index] = c;
            index++;
            printfch(c);
        }
    }
    return str;
}
char readKeys(){
    unsigned char status;
    char keycode;
    while(1){
        status = inportb(0x64);
        if(status & 0x01){
            keycode = inportb(0x60);
            if(keycode < 0){
                continue;
            }
            else{
                break;
            }
        }
    }
    if(keycode < 0x3A){
        if(keycode == 0x0E){
            return '\b';
        }
        else if(keycode == 0x1C){
            return '\n';
        }
        else if(keycode == 0x39){
            return ' ';
        }
        else if(keycode < sizeof(scancode_to_ascii)){
            return scancode_to_ascii[keycode];
        }
    }
    return 0;
}

