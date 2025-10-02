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

