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

#include "../include/shell.h"
#include "../include/snake.h"
#include "../include/memory.h"
#include "../include/fs.h"
#include "../include/process.h"
#include "../include/timer.h"
#include "../include/hal.h"

void launch_shell(int n) {
    set_screen_color(0x0A, 0x00);
    printf("DaOS> ");
    set_screen_color(0x07, 0x00);
    string input = readStr();
    
    string command;
    string arg;
    split_command(input, &command, &arg);
    
    if (cmdEql(command, "help")) {
        printf("Available commands:\n");
        printf("System:\n");
        printf("  help - Show this help message\n");
        printf("  clear - Clear the screen\n");
        printf("  uptime - Show system uptime\n");
        printf("  memstat - Show memory statistics\n");
        printf("  ps - List all processes\n");
        printf("  devices - List registered devices\n");
        printf("Display:\n");
        printf("  color - Change text and background color\n");
        printf("  echo <text> - Echo the input text\n");
        printf("Math:\n");
        printf("  add - Add two integers\n");
        printf("  sub - Subtract two integers\n");
        printf("  mul - Multiply two integers\n");
        printf("  div - Divide two integers\n");
        printf("Files:\n");
        printf("  ls - List all files\n");
        printf("  cat <file> - Display file contents\n");
        printf("  touch <file> - Create a new file\n");
        printf("  write <file> - Write to a file\n");
        printf("  rm <file> - Delete a file\n");
        printf("Games:\n");
        printf("  snake - Play the Snake game\n");
        printf("Other:\n");
        printf("  exit - Exit the shell\n");
    } else if (cmdEql(command, "clear")) {
        clearScreen();
    } else if (cmdEql(command, "color")) {
        printf("Enter text color (0-15): ");
        string textColorStr = readStr();
        int textColor = str_to_int(textColorStr);
        printf("Enter background color (0-15): ");
        string bgColorStr = readStr();
        int bgColor = str_to_int(bgColorStr);
        if (textColor < 0 || textColor > 15 || bgColor < 0 || bgColor > 15) {
            set_screen_color(0x0C, 0x00);
            printf("Error: Color values must be between 0 and 15.\n");
            set_screen_color(0x07, 0x00);
        } else {
            set_screen_color(textColor, bgColor);
            printf("Color changed successfully.\n");
        }
    } else if (cmdEql(command, "echo")) {
        if(strlength(arg) > 0) {
            printf(arg);
            printf("\n");
        } else {
            printf("Usage: echo <text>\n");
        }
    } else if (cmdEql(command, "add")) {
        printf("Enter first integer: ");
        string num1Str = readStr();
        int num1 = str_to_int(num1Str);
        printf("Enter second integer: ");
        string num2Str = readStr();
        int num2 = str_to_int(num2Str);
        int result = num1 + num2;
        printf("Result: ");
        char resultStr[20];
        int_to_ascii(result, resultStr);
        printf(resultStr);
        printf("\n");
    } else if (cmdEql(command, "sub")) {
        printf("Enter first integer: ");
        string num1Str = readStr();
        int num1 = str_to_int(num1Str);
        printf("Enter second integer: ");
        string num2Str = readStr();
        int num2 = str_to_int(num2Str);
        int result = num1 - num2;
        printf("Result: ");
        char resultStr[20];
        int_to_ascii(result, resultStr);
        printf(resultStr);
        printf("\n");
    } else if (cmdEql(command, "mul")) {
        printf("Enter first integer: ");
        string num1Str = readStr();
        int num1 = str_to_int(num1Str);
        printf("Enter second integer: ");
        string num2Str = readStr();
        int num2 = str_to_int(num2Str);
        int result = num1 * num2;
        printf("Result: ");
        char resultStr[20];
        int_to_ascii(result, resultStr);
        printf(resultStr);
        printf("\n");
    } else if (cmdEql(command, "div")) {
        printf("Enter first integer: ");
        string num1Str = readStr();
        int num1 = str_to_int(num1Str);
        printf("Enter second integer: ");
        string num2Str = readStr();
        int num2 = str_to_int(num2Str);
        if (num2 == 0) {
            set_screen_color(0x0C, 0x00);
            printf("Error: Division by zero is not allowed.\n");
            set_screen_color(0x07, 0x00);
        } else {
            int result = num1 / num2;
            printf("Result: ");
            char resultStr[20];
            int_to_ascii(result, resultStr);
            printf(resultStr);
            printf("\n");
        }
    } else if (cmdEql(command, "snake")) {
        play_snake();
    } else if (cmdEql(command, "memstat")) {
        print_memory_stats();
    } else if (cmdEql(command, "uptime")) {
        uint32 ticks = get_tick_count();
        uint32 seconds = ticks / 100;
        uint32 minutes = seconds / 60;
        uint32 hours = minutes / 60;
        
        printf("System uptime: ");
        char h[10], m[10], s[10];
        int_to_ascii(hours, h);
        int_to_ascii(minutes % 60, m);
        int_to_ascii(seconds % 60, s);
        printf(h);
        printf("h ");
        printf(m);
        printf("m ");
        printf(s);
        printf("s\n");
    } else if (cmdEql(command, "ps")) {
        list_processes();
    } else if (cmdEql(command, "devices")) {
        list_devices();
    } else if (cmdEql(command, "ls")) {
        list_files();
    } else if (cmdEql(command, "cat")) {
        if(strlength(arg) > 0) {
            uint32 size;
            char* content = read_file(arg, &size);
            if(content && size > 0) {
                for(uint32 i = 0; i < size; i++) {
                    printfch(content[i]);
                }
                printf("\n");
            } else {
                set_screen_color(0x0C, 0x00);
                printf("Error: File not found or empty.\n");
                set_screen_color(0x07, 0x00);
            }
        } else {
            printf("Usage: cat <filename>\n");
        }
    } else if (cmdEql(command, "touch")) {
        if(strlength(arg) > 0) {
            if(create_file(arg) >= 0) {
                printf("File created: ");
                printf(arg);
                printf("\n");
            } else {
                set_screen_color(0x0C, 0x00);
                printf("Error: Could not create file.\n");
                set_screen_color(0x07, 0x00);
            }
        } else {
            printf("Usage: touch <filename>\n");
        }
    } else if (cmdEql(command, "write")) {
        if(strlength(arg) > 0) {
            printf("Enter content (press Enter to save):\n");
            string content = readStr();
            if(write_file(arg, content, strlength(content)) == 0) {
                printf("File written: ");
                printf(arg);
                printf("\n");
            } else {
                set_screen_color(0x0C, 0x00);
                printf("Error: Could not write file.\n");
                set_screen_color(0x07, 0x00);
            }
        } else {
            printf("Usage: write <filename>\n");
        }
    } else if (cmdEql(command, "rm")) {
        if(strlength(arg) > 0) {
            if(delete_file(arg) == 0) {
                printf("File deleted: ");
                printf(arg);
                printf("\n");
            } else {
                set_screen_color(0x0C, 0x00);
                printf("Error: File not found.\n");
                set_screen_color(0x07, 0x00);
            }
        } else {
            printf("Usage: rm <filename>\n");
        }
    } else if (cmdEql(command, "exit")) {        
        set_screen_color(0x0F, 0x00);
        printf("Exiting shell...\n");
        return;
    } else {
        set_screen_color(0x0C, 0x00);
        printf("Error: Unknown command '");
        printf(command);
        printf("'. Type 'help' for a list of commands.\n");
        set_screen_color(0x07, 0x00);
    }
    launch_shell(n + 1);
}