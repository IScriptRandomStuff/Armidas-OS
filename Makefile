CC     = i686-elf-gcc
ASM    = nasm
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra

OBJS = boot/boot.o \
       kernel/kernel.o \
       kernel/vga/vga.o

# add this rule alongside the other compile rules
kernel/vga/vga.o: kernel/vga/vga.c
	$(CC) $(CFLAGS) -c kernel/vga/vga.c -o kernel/vga/vga.o

all: mykernel.iso

# Compile .c files
kernel/kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c kernel/kernel.c -o kernel/kernel.o

# Assemble .asm files
boot/boot.o: boot/boot.asm
	$(ASM) -f elf32 boot/boot.asm -o boot/boot.o

# Link everything into a binary
mykernel.bin: $(OBJS)
	$(CC) -T linker.ld -o mykernel.bin \
	      -ffreestanding -O2 -nostdlib \
	      $(OBJS) -lgcc

# Package it into a bootable ISO with GRUB
mykernel.iso: mykernel.bin
	mkdir -p isodir/boot/grub
	cp mykernel.bin isodir/boot/mykernel.bin
	echo 'set timeout=0'                          > isodir/boot/grub/grub.cfg
	echo 'set default=0'                         >> isodir/boot/grub/grub.cfg
	echo 'menuentry "MyKernel" {'                >> isodir/boot/grub/grub.cfg
	echo '    multiboot /boot/mykernel.bin'      >> isodir/boot/grub/grub.cfg
	echo '}'                                     >> isodir/boot/grub/grub.cfg
	grub-mkrescue -o mykernel.iso isodir

# Boot it in QEMU
run: mykernel.iso
	qemu-system-i386 -cdrom mykernel.iso

clean:
	rm -f boot/boot.o kernel/kernel.o
	rm -f mykernel.bin mykernel.iso
	rm -rf isodir