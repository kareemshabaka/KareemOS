SRCDIR = src
INCDIR = include
OBJDIR = build

CC = clang
LD = ld.lld
AS = clang
QEMU = qemu-system-i386

# -m32 is crucial for x86 32-bit development in a Linux environment
# -fno-stack-protector is needed until you implement stack smashing protection
CFLAGS = --target=i386-pc-none-elf -I$(INCDIR) -ffreestanding -O2 -Wall -Wextra \
         -mno-sse -m32 -fno-stack-protector -nostdlib -fno-builtin -g

LDFLAGS = -T $(SRCDIR)/linker.ld
ASFLAGS = --target=i386-pc-none-elf -m32 -c

C_SOURCES  = $(wildcard $(SRCDIR)/*.c)
AS_SOURCES = $(wildcard $(SRCDIR)/*.s)

OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(C_SOURCES)) \
       $(patsubst $(SRCDIR)/%.s, $(OBJDIR)/%.o, $(AS_SOURCES))

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

$(OBJDIR)/kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.s
	@mkdir -p $(OBJDIR)
	$(AS) $(ASFLAGS) $< -o $@

QEMU_FLAGS = -kernel $(OBJDIR)/kernel.elf -no-reboot -no-shutdown \
             -action reboot=reset -monitor stdio \
             -d guest_errors,cpu_reset -D qemu.log

run: all
	$(QEMU) $(QEMU_FLAGS)

debug: all
	$(QEMU) $(QEMU_FLAGS) -s -S

clean:
	rm -rf $(OBJDIR)