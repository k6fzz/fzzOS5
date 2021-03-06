
OSNAME = fzzOS

CCDIR := ../crosscompiler/bin

SRCDIR := ./src
OBJDIR := ./obj
BUILDDIR = ./bin
INCDIR := ./include


GNUEFI = ../gnu-efi
LIMINE = ../limine
OVMFDIR = ../OVMFbin
UTILDIR = ../util
INITDIR = ../init

LIMINEEFI := $(LIMINE)/main.efi
LIMINE3EFI := $(LIMINE)/limine3/BOOTX64.EFI

LDS = $(SRCDIR)/kernel.ld
CC = $(CCDIR)/x86_64-elf-gcc 
LD = $(CCDIR)/x86_64-elf-ld
STRIP = x86_64-elf-strip
ASMC = nasm

TARC = tar
TARFLAGS ?= -cf

#CFLAGS = -fpic -ffreestanding -fno-stack-protector -fshort-wchar -mno-red-zone -fno-exceptions -g
CFLAGS ?= -O2 -g -Wall -Wextra -pipe -I$(INCDIR) -D__DEBUG__
override INTERNALCFLAGS := -I. \
	-std=gnu11           \
	-ffreestanding       \
	-fno-stack-protector \
	-fno-pic             \
	-mabi=sysv           \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mno-sse             \
	-mno-sse2            \
	-mno-red-zone        \
	-mcmodel=kernel      \
	-MMD
ASMFLAGS = 
#LDFLAGS = -T $(LDS) -nostdlib -nostartfiles
LDFLAGS ?=
override INTERNALLDFLAGS :=    \
	-T $(LDS)         \
	-nostdlib              \
	-zmax-page-size=0x1000 \
	-static
STRIPFLAGS =  -s -K mmio -K fb -K bootboot -K environment -K initstack -K _KernelStart -K _KernelEnd



rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)
CSRC = $(call rwildcard,$(SRCDIR),*.c)
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)
GASMSRC = $(call rwildcard,$(SRCDIR),*.S)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%_cpp.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))
OBJS += $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%_c.o, $(CSRC))
OBJS += $(patsubst $(SRCDIR)/%.S, $(OBJDIR)/%_S.o, $(GASMSRC))
DIRS = $(wildcard $(SRCDIR)/*)

INITRD = $(call rwildcard,$(INITRDDIR),*.*)

kernel: $(OBJS) link

#$(OBJDIR)/interrupts/interrupts.o: $(SRCDIR)/interrupts/interrupts.cpp
#	@ echo !==== Compiling $^
#	@ mkdir -p $(@D)
#	$(CC) -mno-red-zone -mgeneral-regs-only -ffreestanding -c $^ -o $@

$(OBJDIR)/%_c.o: $(SRCDIR)/%.c
	@ echo !==== Compiling $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c $^ -o $@

$(OBJDIR)/%_cpp.o: $(SRCDIR)/%.cpp
	@ echo !==== Compiling $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c $^ -o $@

$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ echo !==== Compiling $^
	@ mkdir -p $(@D)
	$(ASMC) $(ASMFLAGS) $^ -f elf64 -o $@

$(OBJDIR)/%_S.o: $(SRCDIR)/%.s
	@ echo !==== Compiling $^
	@ mkdir -p $(@D)
	$(CC) -masm=intel $(CFLAGS) $(INTERNALCFLAGS)  -c $^ -o $@
	
link:
	@ echo !==== Linking!
	$(LD) $(LDFLAGS) $(INTERNALLDFLAGS) -o $(BUILDDIR)/kernel.elf $(OBJS) zapl16.o
	#$(STRIP) $(STRIPFLAGS) $(BUILDDIR)/kernel.elf
	readelf -hls $(BUILDDIR)/kernel.elf>kernel.txt
	objdump -S --disassemble $(BUILDDIR)/kernel.elf>assemble.txt
	#tar -cvf $(BUILDDIR)/initrd $(BUILDDIR)/$(INITRD)


setup:
	@mkdir $(BUILDDIR)
	@mkdir $(SRCDIR)
	@mkdir $(OBJDIR)

buildimg:
	dd if=/dev/zero of=$(BUILDDIR)/$(OSNAME).img bs=512 count=2016
	mformat -i $(BUILDDIR)/$(OSNAME).img ::
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(LIMINEEFI) ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(LIMINE)/limine.cfg ::/EFI/BOOT/
	mcopy -i $(BUILDDIR)/$(OSNAME).img startup.nsh ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/kernel.elf ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(INITDIR)/init.elf ::

buildlimine:
	dd if=/dev/zero of=$(BUILDDIR)/$(OSNAME).img bs=512 count=2016
	mformat -i $(BUILDDIR)/$(OSNAME).img ::
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(LIMINE3EFI) ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(LIMINE)/limine3/limine.cfg ::/EFI/BOOT/
	mcopy -i $(BUILDDIR)/$(OSNAME).img startup.nsh ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/kernel.elf ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(INITDIR)/init.elf ::

.PHONY: clean
clean:
	rm -rf $(OBJDIR)/*

run: kernel buildimg
	qemu-system-x86_64 -drive file=$(BUILDDIR)/$(OSNAME).img -d cpu_reset -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMFDIR)/OVMF_VARS-pure-efi.fd" -net none -no-reboot -no-shutdown

run-serial: kernel buildimg
	qemu-system-x86_64 -drive file=$(BUILDDIR)/$(OSNAME).img -d cpu_reset -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMFDIR)/OVMF_VARS-pure-efi.fd" -net none -no-reboot -no-shutdown -serial file:seriallog.txt