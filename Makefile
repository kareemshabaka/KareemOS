SRCDIR = src
INCDIR = include
BUILDDIR = build

CC = clang
LD = ld.lld
AS = clang
QEMU = qemu-system-i386

# -m32 is crucial for x86 32-bit development in a Linux environment
# -fno-stack-protector is needed until you implement stack smashing protection
BASE_CFLAGS = --target=i386-pc-none-elf -I$(INCDIR) -ffreestanding -Wall -Wextra \
              -mno-sse -m32 -fno-stack-protector -nostdlib -fno-builtin -g

CFLAGS = $(BASE_CFLAGS) -O2

LDFLAGS = -T $(SRCDIR)/linker.ld
ASFLAGS = --target=i386-pc-none-elf -m32 -c

CONFIG ?= release

ifeq ($(CONFIG), debug)
    CFLAGS = $(BASE_CFLAGS) -O0
    OBJDIR = $(BUILDDIR)/debug
else
    CFLAGS = $(BASE_CFLAGS) -O2
    OBJDIR = $(BUILDDIR)/release
endif

C_SOURCES  = $(wildcard $(SRCDIR)/*.c)
AS_SOURCES = $(wildcard $(SRCDIR)/*.s)

OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(C_SOURCES)) \
       $(patsubst $(SRCDIR)/%.s, $(OBJDIR)/%.o, $(AS_SOURCES))

%.o: %.c $(HEADERS)
	$(CC) -c $< -o $@ $(CFLAGS)

lint:
	clang-tidy $(C_SOURCES) -- \
		-target i386-pc-none-elf \
		-march=i386 \
		-m32 \
		-ffreestanding \
		-Iinclude \
		-I/usr/lib/llvm-14/lib/clang/14.0.0/include

format:
	clang-format -i $(C_SOURCES) $(wildcard $(INCDIR)/*.h)

all: $(OBJDIR)/kernel.elf
	rm -f qemu.log

$(OBJDIR)/kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.s
	@mkdir -p $(OBJDIR)
	$(AS) $(ASFLAGS) $< -o $@

QEMU_FLAGS = -no-reboot -no-shutdown \
             -action reboot=reset -monitor stdio \
             -d guest_errors,cpu_reset -D qemu.log

clean:
	rm -rf $(BUILDDIR)

run:
	$(MAKE) CONFIG=release all
	$(QEMU) -kernel $(BUILDDIR)/release/kernel.elf $(QEMU_FLAGS)

debug:
	$(MAKE) CONFIG=debug all
	$(QEMU) -kernel $(BUILDDIR)/debug/kernel.elf $(QEMU_FLAGS) -s -S
