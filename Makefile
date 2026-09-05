CC = gcc
LD = ld
NASM = nasm

CFLAGS = -ffreestanding -m64 -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -Wall -Wextra -Ikernel
LDFLAGS = -nostdlib -z max-page-size=0x1000 -T linker.ld

BUILD = build

C_SOURCES = \
	kernel/kernel.c \
	kernel/cpu/idt.c \
	kernel/cpu/pic.c \
	kernel/cpu/timer.c \
	kernel/sched.c

C_OBJECTS = $(C_SOURCES:%.c=$(BUILD)/%.o)

ASM_SOURCES = \
	boot/entry.asm \
	kernel/cpu/isr.asm

ASM_OBJECTS = \
	$(BUILD)/boot/entry.o \
	$(BUILD)/kernel/cpu/isr.o

all: $(BUILD)/satos.iso

$(BUILD)/satos.elf: $(C_OBJECTS) $(ASM_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD)/satos.iso: $(BUILD)/satos.elf boot/grub.cfg
	mkdir -p $(BUILD)/iso/boot/grub
	cp $(BUILD)/satos.elf $(BUILD)/iso/boot/satos.elf
	cp boot/grub.cfg $(BUILD)/iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ $(BUILD)/iso

$(BUILD)/kernel/%.o: kernel/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/boot/entry.o: boot/entry.asm
	mkdir -p $(dir $@)
	$(NASM) -f elf64 $< -o $@

$(BUILD)/kernel/cpu/isr.o: kernel/cpu/isr.asm
	mkdir -p $(dir $@)
	$(NASM) -f elf64 $< -o $@

run: $(BUILD)/satos.iso
	qemu-system-x86_64 -cdrom $(BUILD)/satos.iso

clean:
	rm -rf $(BUILD)