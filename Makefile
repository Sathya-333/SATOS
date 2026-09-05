ASM=nasm
CC=gcc
LD=ld

CFLAGS=-std=gnu11 \
       -ffreestanding \
       -O2 \
       -Wall \
       -Wextra \
       -m64 \
       -mno-red-zone \
       -fno-pie \
       -fno-stack-protector \
       -mno-sse \
       -mno-sse2 \
       -mno-mmx \
       -mno-80387

LDFLAGS=-T linker.ld -nostdlib

BUILD=build

KERNEL=$(BUILD)/satos.elf
ISO=$(BUILD)/satos.iso

.PHONY: all clean run debug

all: $(ISO)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/entry.o: boot/entry.asm | $(BUILD)
	$(ASM) -f elf64 $< -o $@

$(BUILD)/kernel.o: kernel/kernel.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@
$(BUILD)/idt.o: kernel/cpu/idt.c | $(BUILD)
	$(CC) $(CFLAGS) -Ikernel -c $< -o $@

$(BUILD)/isr.o: kernel/cpu/isr.asm | $(BUILD)
	$(ASM) -f elf64 $< -o $@
$(KERNEL): $(BUILD)/entry.o $(BUILD)/kernel.o $(BUILD)/idt.o $(BUILD)/isr.o linker.ld
	$(LD) $(LDFLAGS) -o $@ $(BUILD)/entry.o $(BUILD)/kernel.o $(BUILD)/idt.o $(BUILD)/isr.o

$(ISO): $(KERNEL) boot/grub.cfg
	mkdir -p $(BUILD)/isodir/boot/grub
	cp $(KERNEL) $(BUILD)/isodir/boot/satos.elf
	cp boot/grub.cfg $(BUILD)/isodir/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(BUILD)/isodir

run: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO)

debug: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO) -s -S

clean:
	rm -rf $(BUILD)