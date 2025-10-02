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

#ifndef SCREEN_H
#define SCREEN_H
#include "system.h"
#include "string.h"

extern int cursorX, cursorY;
extern const uint8 sw, sh;
extern uint8 sd;

void clearLine(uint8 from, uint8 to);

void updateCursor();

void clearScreen();

void scrollUp(uint8 lineNumber);

void newLineCheck();

void printfch(char c);

void printf (string ch);
void set_screen_color_from_color_code(int color_code);
void set_screen_color(int text_color,int bg_color);
void printf_colored(string ch,int text_color,int bg_color);

#endif