#include "screen.h"
#include "ports.h"

int get_cursor_offset(void) {
    outb(REG_SCREEN_CTRL, 14);
    int offset = inb(REG_SCREEN_DATA) << 8;
    outb(REG_SCREEN_CTRL, 15);
    offset += inb(REG_SCREEN_DATA);
    return offset * 2;
}

void set_cursor_offset(int offset) {
    offset /= 2;
    outb(REG_SCREEN_CTRL, 14);
    outb(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
    outb(REG_SCREEN_CTRL, 15);
    outb(REG_SCREEN_DATA, (unsigned char)(offset & 0xff));
}

int handle_scrolling(int cursor_offset) {
    if (cursor_offset < MAX_ROWS * MAX_COLS * 2) {
        return cursor_offset;
    }
    
    // Copy rows back one
    uint8_t* vidmem = (uint8_t*) VIDEO_ADDRESS;
    for (int i = 1; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS * 2; j++) {
            vidmem[(i - 1) * MAX_COLS * 2 + j] = vidmem[i * MAX_COLS * 2 + j];
        }
    }
    
    // Blank last line
    int last_line = (MAX_ROWS - 1) * MAX_COLS * 2;
    for (int i = 0; i < MAX_COLS * 2; i += 2) {
        vidmem[last_line + i] = ' ';
        vidmem[last_line + i + 1] = WHITE_ON_BLACK;
    }
    
    return last_line;
}

void print_char(char character, int col, int row, char attribute_byte) {
    unsigned char *vidmem = (unsigned char *) VIDEO_ADDRESS;
    if (!attribute_byte) {
        attribute_byte = WHITE_ON_BLACK;
    }
    int offset;
    if (col >= 0 && row >= 0) {
        offset = (row * MAX_COLS + col) * 2;
    } else {
        offset = get_cursor_offset();
    }
    
    if (character == '\n') {
        int rows = offset / (2 * MAX_COLS);
        offset = (rows + 1) * 2 * MAX_COLS;
    } else {
        vidmem[offset] = character;
        vidmem[offset + 1] = attribute_byte;
        offset += 2;
    }
    offset = handle_scrolling(offset);
    set_cursor_offset(offset);
}

void print_string(const char* message) {
    int i = 0;
    while (message[i] != 0) {
        print_char(message[i++], -1, -1, 0);
    }
}

void print_nl(void) {
    print_char('\n', -1, -1, 0);
}

void print_backspace(void) {
    int offset = get_cursor_offset() - 2;
    if (offset < 0) return;
    int row = (offset / 2) / MAX_COLS;
    int col = (offset / 2) % MAX_COLS;
    print_char(' ', col, row, WHITE_ON_BLACK);
    set_cursor_offset(offset);
}

void clear_screen(void) {
    int screen_size = MAX_COLS * MAX_ROWS;
    char *screen = (char *) VIDEO_ADDRESS;
    for (int i = 0; i < screen_size; i++) {
        screen[i * 2] = ' ';
        screen[i * 2 + 1] = WHITE_ON_BLACK;
    }
    set_cursor_offset(0);
}

void screen_init(void) {
    clear_screen();
}
