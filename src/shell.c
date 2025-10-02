#include "../include/shell.h"
void launch_shell(int n) {
    set_screen_color(0x0A, 0x00);
    printf("DaOS> ");
    set_screen_color(0x07, 0x00);
    string command = readStr();
    if (cmdEql(command, "help")) {
        printf("Available commands:\n");
        printf("help - Show this help message\n");
        printf("clear - Clear the screen\n");
        printf("color - Change text and background color\n");
        printf("echo - Echo the input text\n");
        printf("add - Add two integers\n");
        printf("sub - Subtract two integers\n");
        printf("mul - Multiply two integers\n");
        printf("div - Divide two integers\n");
        printf("exit - Exit the shell\n");
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
        printf("Enter text to echo: ");
        string echoText = readStr();
        printf(echoText);
        printf("\n");
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