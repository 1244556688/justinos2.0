#include "keyboard.h"
#include "ports.h"
#include "screen.h"
#include "shell.h"
#include <stdint.h>

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_lo = base & 0xFFFF;
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

// US QWERTY Scancode to ASCII map (Set 1)
const char scancode_to_ascii[] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
    0, '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0, ' '
};

// Keyboard Interrupt Handler
__attribute__((interrupt)) void keyboard_handler(void* frame) {
    (void)frame;
    uint8_t scancode = inb(0x60);
    if (scancode < 0x80) { // Key press
        if (scancode == 0x1C) { // Enter
            shell_handle_enter();
        } else if (scancode == 0x0E) { // Backspace
            shell_handle_backspace();
        } else {
            char ascii = scancode < sizeof(scancode_to_ascii) ? scancode_to_ascii[scancode] : 0;
            if (ascii) {
                shell_handle_char(ascii);
            }
        }
    }
    
    // Send EOI (End of Interrupt) to PIC1
    outb(PIC1_COMMAND, 0x20);
}

void keyboard_init(void) {
    // 1. Initialize PIC and Remap IRQs
    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    outb(PIC1_DATA, 0x20); // Master PIC ISR 32..39
    outb(PIC2_DATA, 0x28); // Slave PIC ISR 40..47
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    outb(PIC1_DATA, 0xFD); // Disable all except IRQ1 (Keyboard)
    outb(PIC2_DATA, 0xFF);

    // 2. Setup IDT
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;
    
    // Clear all gates
    for(int i=0; i<256; i++) {
        idt_set_gate(i, 0, 0x08, 0x8E);
    }
    
    // set int 33 (IRQ1) to keyboard_handler
    idt_set_gate(33, (uint32_t)keyboard_handler, 0x08, 0x8E);
    
    // Load IDT and enable interrupts
    __asm__ volatile("lidtl (%0)" : : "r" (&idtp));
    __asm__ volatile("sti");
}
