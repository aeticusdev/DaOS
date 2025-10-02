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

#include "../include/util.h"

void memory_copy(char *source, char *dest, int nbytes) {
    for (int i = 0; i < nbytes; i++) {
        dest[i] = source[i];
    }
}
void memory_set(uint8 *dest, uint8 val, uint32 len) {
    for (uint32 i = 0; i < len; i++) {
        dest[i] = val;
    }
}
void int_to_ascii(int n, char str[]) {  
    int i = 0;
    int isNegative = 0;
    if (n < 0) {
        isNegative = 1;
        n = -n;
    }
    do {
        str[i++] = (n % 10) + '0';
        n /= 10;
    } while (n != 0);
    if (isNegative) {
        str[i++] = '-';
    }
    str[i] = '\0';
    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}  
int str_to_int(string ch)  {
    int result = 0;
    int sign = 1;
    int i = 0;
    if (ch[0] == '-') {
        sign = -1;
        i = 1;
    }
    for (; ch[i] != '\0'; i++) {
        if (ch[i] < '0' || ch[i] > '9') {
            return 0;
        }
        result = result * 10 + (ch[i] - '0');
    }
    return sign * result;
}
void * malloc(int nbytes) {      
    static uint8 *heap = (uint8 *)0x200000;
    uint8 *prev_heap = heap;
    heap += nbytes;
    return prev_heap;
}

