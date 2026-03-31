#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0f
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

void screen_init(void);
void clear_screen(void);
void set_cursor_offset(int offset);
int get_cursor_offset(void);
void print_char(char character, int col, int row, char attribute_byte);
void print_string(const char* message);
void print_nl(void);
void print_backspace(void);

#endif
