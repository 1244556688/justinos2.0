#include "shell.h"
#include "screen.h"
#include "ports.h"

#define MAX_COMMAND_LEN 256
static char command_buffer[MAX_COMMAND_LEN];
static int command_len = 0;

static void print_prompt(void) {
    print_string("justinOS> ");
}

static int strcmp(const char *s1, const char *s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void shell_init(void) {
    command_len = 0;
    print_prompt();
}

void shell_handle_char(char c) {
    if (command_len < MAX_COMMAND_LEN - 1) {
        command_buffer[command_len++] = c;
        char str[2] = {c, '\0'};
        print_string(str);
    }
}

void shell_handle_backspace(void) {
    if (command_len > 0) {
        command_buffer[--command_len] = '\0';
        print_backspace();
    }
}

static void parse_command(void) {
    command_buffer[command_len] = '\0';
    
    if (command_len == 0) {
        return;
    }

    if (strcmp(command_buffer, "help") == 0) {
        print_string("Available commands:\n");
        print_string("help     - Show all commands\n");
        print_string("clear    - Clear screen\n");
        print_string("echo     - Print text (e.g. echo hello)\n");
        print_string("time     - Show system time\n");
        print_string("version  - Show OS version\n");
        print_string("reboot   - Reboot system (QEMU)\n");
        print_string("shutdown - Shutdown system (QEMU)\n");
        print_string("about    - Show system info\n");
    } 
    else if (strcmp(command_buffer, "clear") == 0) {
        clear_screen();
    }
    else if (command_buffer[0]=='e' && command_buffer[1]=='c' && command_buffer[2]=='h' && command_buffer[3]=='o') {
        if(command_buffer[4] == ' ') {
            print_string(&command_buffer[5]);
        }
        print_nl();
    }
    else if (strcmp(command_buffer, "time") == 0) {
        print_string("Time: [Simulated] 12:00:00\n");
    }
    else if (strcmp(command_buffer, "version") == 0) {
        print_string("justinOS v0.1 x86 32-bit\n");
    }
    else if (strcmp(command_buffer, "about") == 0) {
        print_string("justinOS is a custom OS built from scratch.\n");
        print_string("Language: C and NASM.\n");
    }
    else if (strcmp(command_buffer, "reboot") == 0) {
        outb(0x64, 0xFE); // Pulse reset line via keyboard controller
        print_string("Rebooting...\n");
        while(1) { __asm__ volatile("hlt"); }
    }
    else if (strcmp(command_buffer, "shutdown") == 0) {
        outb(0xf4, 0x00); // QEMU ACPI shutdown
        print_string("Shutting down... Please power off manually if on real hardware.\n");
        while(1) { __asm__ volatile("hlt"); }
    }
    else {
        print_string("Unknown command. Type 'help'.\n");
    }
}

void shell_handle_enter(void) {
    print_nl();
    parse_command();
    
    // Setup for next command
    int was_clear = (strcmp(command_buffer, "clear") == 0);
    command_len = 0;
    command_buffer[0] = '\0';
    
    if (!was_clear) {
        print_prompt();
    } else {
        // clear_screen already resets cursor to 0,0. We just print prompt.
        print_prompt();
    }
}
