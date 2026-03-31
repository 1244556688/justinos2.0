#include "screen.h"
#include "keyboard.h"
#include "shell.h"

void kernel_main(void) {
    // 1. Initialize Screen
    screen_init();
    print_string("Welcome to justinOS!\n");
    print_string("Initializing Core Systems...\n");
    
    // 2. Initialize Keyboard and Interrupts
    keyboard_init();
    print_string("Systems Initialized.\n\n");
    
    // 3. Start Shell
    shell_init();
    
    // 4. Halt loop (save CPU)
    while(1) {
        __asm__ volatile("hlt");
    }
}
