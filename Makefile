CC     := i686-linux-gnu-gcc
ASM    := nasm
CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra

# Automatically find all C and assembly source files
SRC_C   := $(shell find src -name '*.c')
SRC_ASM := $(shell find src -name '*.asm')
OBJS    := $(SRC_C:.c=.o) $(SRC_ASM:.asm=.o)

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble NASM files
%.o: %.asm
	$(ASM) -f elf32 $< -o $@

# Default target
all: mykernel.iso

# Link everything into a kernel binary
mykernel.bin: $(OBJS)
	$(CC) -T linker.ld -o $@ \
	      -ffreestanding -O2 -nostdlib \
	      $(OBJS) -lgcc

# Package into a bootable ISO
mykernel.iso: mykernel.bin
	mkdir -p isodir/boot/grub
	cp $< isodir/boot/mykernel.bin
	echo 'set timeout=0'                     > isodir/boot/grub/grub.cfg
	echo 'set default=0'                    >> isodir/boot/grub/grub.cfg
	echo 'menuentry "AmidasOS" {'           >> isodir/boot/grub/grub.cfg
	echo '    multiboot /boot/mykernel.bin' >> isodir/boot/grub/grub.cfg
	echo '}'                                >> isodir/boot/grub/grub.cfg
	grub-mkrescue -o $@ isodir

run: mykernel.iso
	qemu-system-i386 -cdrom $<

clean:
	rm -f $(OBJS) mykernel.bin mykernel.iso
	rm -rf isodir

.PHONY: all run clean