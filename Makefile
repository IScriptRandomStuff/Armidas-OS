CC     = i686-linux-gnu-gcc
ASM    = nasm
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra

OBJS = src/boot/boot.o \
       src/kernel/kernel.o \
       src/drivers/vga.o \
       src/arch/x86/gdt.o \
       src/arch/x86/idt.o

# add this compile rule
src/arch/x86/idt.o: src/arch/x86/idt.c
	$(CC) $(CFLAGS) -c src/arch/x86/idt.c -o src/arch/x86/idt.o

all: mykernel.iso

# Assemble .asm files
src/boot/boot.o: src/boot/boot.asm
	$(ASM) -f elf32 src/boot/boot.asm -o src/boot/boot.o

# Compile .c files
src/kernel/kernel.o: src/kernel/kernel.c
	$(CC) $(CFLAGS) -c src/kernel/kernel.c -o src/kernel/kernel.o

src/drivers/vga.o: src/drivers/vga.c
	$(CC) $(CFLAGS) -c src/drivers/vga.c -o src/drivers/vga.o

src/arch/x86/gdt.o: src/arch/x86/gdt.c
	$(CC) $(CFLAGS) -c src/arch/x86/gdt.c -o src/arch/x86/gdt.o

# Link everything into a binary
mykernel.bin: $(OBJS)
	$(CC) -T linker.ld -o mykernel.bin \
	      -ffreestanding -O2 -nostdlib \
	      $(OBJS) -lgcc

# Package into a bootable ISO
mykernel.iso: mykernel.bin
	mkdir -p isodir/boot/grub
	cp mykernel.bin isodir/boot/mykernel.bin
	echo 'set timeout=0'                          > isodir/boot/grub/grub.cfg
	echo 'set default=0'                         >> isodir/boot/grub/grub.cfg
	echo 'menuentry "AmidasOS" {'                >> isodir/boot/grub/grub.cfg
	echo '    multiboot /boot/mykernel.bin'      >> isodir/boot/grub/grub.cfg
	echo '}'                                     >> isodir/boot/grub/grub.cfg
	grub-mkrescue -o mykernel.iso isodir

run: mykernel.iso
	qemu-system-i386 -cdrom mykernel.iso

clean:
	rm -f src/boot/boot.o
	rm -f src/kernel/kernel.o
	rm -f src/drivers/vga.o
	rm -f src/arch/x86/gdt.o
	rm -f src/arch/x86/idt.o
	rm -f mykernel.bin mykernel.iso
	rm -rf isodir