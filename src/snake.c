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

#include "../include/snake.h"

#define DIR_UP 1
#define DIR_DOWN 2
#define DIR_LEFT 3
#define DIR_RIGHT 4

static Snake snake;
static Food food;
static int game_over;
static int score;
static uint32 tick_counter;

void draw_border() {
    int startX = 20;
    int startY = 2;
    
    set_screen_color(0x09, 0x00);
    
    for(int x = 0; x < GAME_WIDTH + 2; x++) {
        cursorX = startX + x;
        cursorY = startY;
        updateCursor();
        printfch('#');
        
        cursorX = startX + x;
        cursorY = startY + GAME_HEIGHT + 1;
        updateCursor();
        printfch('#');
    }
    
    for(int y = 1; y < GAME_HEIGHT + 1; y++) {
        cursorX = startX;
        cursorY = startY + y;
        updateCursor();
        printfch('#');
        
        cursorX = startX + GAME_WIDTH + 1;
        cursorY = startY + y;
        updateCursor();
        printfch('#');
    }
    
    set_screen_color(0x0F, 0x00);
}

void init_snake() {
    snake.length = 4;
    snake.direction = DIR_RIGHT;
    
    for(int i = 0; i < snake.length; i++) {
        snake.body[i].x = 15 - i;
        snake.body[i].y = 10;
    }
}

void spawn_food() {
    int attempts = 0;
    while(attempts < 100) {
        food.position.x = ((tick_counter + attempts * 13) % (GAME_WIDTH - 2)) + 1;
        food.position.y = (((tick_counter + attempts) * 7) % (GAME_HEIGHT - 2)) + 1;
        
        int collision = 0;
        for(int i = 0; i < snake.length; i++) {
            if(snake.body[i].x == food.position.x && snake.body[i].y == food.position.y) {
                collision = 1;
                break;
            }
        }
        
        if(!collision) {
            food.active = 1;
            return;
        }
        attempts++;
    }
    food.active = 1;
}

void draw_snake() {
    int startX = 20;
    int startY = 2;
    
    for(int i = 0; i < snake.length; i++) {
        cursorX = startX + snake.body[i].x + 1;
        cursorY = startY + snake.body[i].y + 1;
        updateCursor();
        if(i == 0) {
            set_screen_color(0x0A, 0x00);
            printfch('O');
        } else {
            set_screen_color(0x02, 0x00);
            printfch('o');
        }
    }
    set_screen_color(0x0F, 0x00);
}

void draw_food() {
    int startX = 20;
    int startY = 2;
    
    if(food.active) {
        cursorX = startX + food.position.x + 1;
        cursorY = startY + food.position.y + 1;
        updateCursor();
        set_screen_color(0x0C, 0x00);
        printfch('*');
        set_screen_color(0x0F, 0x00);
    }
}

void clear_game_area() {
    int startX = 20;
    int startY = 2;
    
    for(int y = 1; y < GAME_HEIGHT + 1; y++) {
        for(int x = 1; x < GAME_WIDTH + 1; x++) {
            cursorX = startX + x;
            cursorY = startY + y;
            updateCursor();
            printfch(' ');
        }
    }
}

int check_collision() {
    Point head = snake.body[0];
    
    if(head.x < 0 || head.x >= GAME_WIDTH || head.y < 0 || head.y >= GAME_HEIGHT) {
        return 1;
    }
    
    for(int i = 1; i < snake.length; i++) {
        if(snake.body[i].x == head.x && snake.body[i].y == head.y) {
            return 1;
        }
    }
    
    return 0;
}

void move_snake() {
    Point new_head = snake.body[0];
    
    if(snake.direction == DIR_UP) {
        new_head.y--;
    } else if(snake.direction == DIR_DOWN) {
        new_head.y++;
    } else if(snake.direction == DIR_LEFT) {
        new_head.x--;
    } else if(snake.direction == DIR_RIGHT) {
        new_head.x++;
    }
    
    for(int i = snake.length - 1; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
    }
    
    snake.body[0] = new_head;
    
    if(food.active && new_head.x == food.position.x && new_head.y == food.position.y) {
        score += 10;
        food.active = 0;
        if(snake.length < MAX_SNAKE_LENGTH) {
            snake.length++;
        }
    }
}

void update_score() {
    cursorX = 1;
    cursorY = 1;
    updateCursor();
    set_screen_color(0x0E, 0x00);
    printf("Score: ");
    
    char score_str[10];
    int temp = score;
    int digits = 0;
    
    if(temp == 0) {
        score_str[digits++] = '0';
    } else {
        while(temp > 0) {
            score_str[digits++] = (temp % 10) + '0';
            temp /= 10;
        }
    }
    
    for(int i = digits - 1; i >= 0; i--) {
        printfch(score_str[i]);
    }
    set_screen_color(0x0F, 0x00);
}

void delay(uint32 ms) {
    for(uint32 i = 0; i < ms * 10000; i++) {
        __asm__ __volatile__("nop");
    }
}

void play_snake() {
    clearScreen();
    game_over = 0;
    score = 0;
    tick_counter = 42;
    
    init_snake();
    spawn_food();
    
    cursorX = 1;
    cursorY = 0;
    updateCursor();
    set_screen_color(0x0B, 0x00);
    printf("SNAKE GAME - Use WASD or Arrow Keys - ESC to quit");
    set_screen_color(0x0F, 0x00);
    
    delay(50);
    
    uint32 frame_counter = 0;
    
    while(!game_over) {
        
        uint8 key = read_scancode();
        
        if(key == KEY_W || key == KEY_UP) {
            if(snake.direction != DIR_DOWN) {
                snake.direction = DIR_UP;
            }
        } else if(key == KEY_S || key == KEY_DOWN) {
            if(snake.direction != DIR_UP) {
                snake.direction = DIR_DOWN;
            }
        } else if(key == KEY_A || key == KEY_LEFT) {
            if(snake.direction != DIR_RIGHT) {
                snake.direction = DIR_LEFT;
            }
        } else if(key == KEY_D || key == KEY_RIGHT) {
            if(snake.direction != DIR_LEFT) {
                snake.direction = DIR_RIGHT;
            }
        } else if(key == KEY_ESC) {
            break;
        }
        
        frame_counter++;
        if(frame_counter >= 500) {
            move_snake();
            
            if(check_collision()) {
                break;
            }
            
            draw_border();
            update_score();
            
            clear_game_area();
            draw_food();
            draw_snake();
            
            if(!food.active) {
                spawn_food();
            }
            
            frame_counter = 0;
        }
        
        tick_counter++;
        delay(5);
    }
    
    clearScreen();
    set_screen_color(0x0C, 0x00);
    printf("Game Over! ");
    set_screen_color(0x0E, 0x00);
    printf("Final Score: ");
    
    char score_str[10];
    int temp = score;
    int digits = 0;
    
    if(temp == 0) {
        score_str[digits++] = '0';
    } else {
        while(temp > 0) {
            score_str[digits++] = (temp % 10) + '0';
            temp /= 10;
        }
    }
    
    for(int i = digits - 1; i >= 0; i--) {
        printfch(score_str[i]);
    }
    
    printfch('\n');
    set_screen_color(0x07, 0x00);
    printf("Press any key to continue...\n");
    readKeys();
    clearScreen();
}
