CC = gcc
LD = ld

CFLAGS = -m32 -ffreestanding -fno-pie -nostdlib -mgeneral-regs-only -Wall -Wextra -I../kernel
LDFLAGS = -m elf_i386 -T linker.ld --oformat binary

C_SOURCES = $(wildcard ../kernel/*.c)
OBJ = $(C_SOURCES:.c=.o)

all: justinOS.bin

justinOS.bin: boot.bin kernel.bin
	cat boot.bin kernel.bin > justinOS.bin

boot.bin: ../boot/boot.asm
	nasm $< -f bin -o $@

../kernel/kernel_entry.o: ../kernel/kernel_entry.asm
	nasm $< -f elf32 -o $@

kernel.bin: ../kernel/kernel_entry.o $(OBJ)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

qemu: justinOS.bin
	qemu-system-i386 -drive format=raw,file=justinOS.bin,index=0,if=floppy

clean:
	rm -f *.bin ../kernel/*.o
