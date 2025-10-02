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

#include "../include/string.h"
#include "../include/util.h"
#include "../include/screen.h"

uint16 strlength(string ch) {
    uint16 len = 0;
    while (ch[len] != '\0') {
        len++;
    }
    return len;
}

uint8 strEql(string ch1, string ch2) {
    uint16 len1 = strlength(ch1);
    uint16 len2 = strlength(ch2);
    if (len1 != len2) {
        return 0;
    }
    for (uint16 i = 0; i < len1; i++) {
        if (ch1[i] != ch2[i]) {
            return 0;
        }
    }
    return 1;
}

uint8 cmdEql(string ch1, string ch2) {
    uint16 len1 = strlength(ch1);
    uint16 len2 = strlength(ch2);
    if (len1 != len2) {
        return 0;
    }
    for (uint16 i = 0; i < len1; i++) {
        char c1 = ch1[i];
        char c2 = ch2[i];
        if (c1 >= 'A' && c1 <= 'Z') {
            c1 += 32; 
        }
        if (c2 >= 'A' && c2 <= 'Z') {
            c2 += 32; 
        }
        if (c1 != c2) {
            return 0;
        }
    }
    return 1;
}

uint8 cmdLength(string ch) {
    uint16 len = 0;
    while (ch[len] != '\0' && ch[len] != ' ') {
        len++;
    }
    return len;
}

void strcpy(char* dest, const char* src) {
    int i = 0;
    while(src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int strcmp(const char* str1, const char* str2) {
    int i = 0;
    while(str1[i] != '\0' && str2[i] != '\0') {
        if(str1[i] != str2[i]) {
            return str1[i] - str2[i];
        }
        i++;
    }
    return str1[i] - str2[i];
}

void split_command(string input, string* cmd, string* arg) {
    int i = 0;
    while(input[i] != '\0' && input[i] != ' ') {
        i++;
    }
    
    *cmd = (string)malloc(i + 1);
    for(int j = 0; j < i; j++) {
        (*cmd)[j] = input[j];
    }
    (*cmd)[i] = '\0';
    
    while(input[i] == ' ') {
        i++;
    }
    
    int arg_start = i;
    int arg_len = 0;
    while(input[i] != '\0') {
        arg_len++;
        i++;
    }
    
    *arg = (string)malloc(arg_len + 1);
    for(int j = 0; j < arg_len; j++) {
        (*arg)[j] = input[arg_start + j];
    }
    (*arg)[arg_len] = '\0';
}
