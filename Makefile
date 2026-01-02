# Directories
SRC_DIR := src
BUILD_DIR := build
ISO_DIR := $(BUILD_DIR)/iso
GRUB_DIR := $(ISO_DIR)/boot/grub

# Source files
C_SOURCES := $(wildcard $(SRC_DIR)/*.c)
ASM_SOURCES := $(wildcard $(SRC_DIR)/*.asm)
LINKER_SCRIPT := $(SRC_DIR)/linker.ld

# Object files
C_OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
ASM_OBJS := $(patsubst $(SRC_DIR)/%.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
KERNEL_BIN := $(BUILD_DIR)/kernel.bin
ISO := $(BUILD_DIR)/kernel.iso

# Compiler flags
CFLAGS := -m32 -ffreestanding -nostdlib -fno-pie -fno-pic
LDFLAGS := -m elf_i386 -T $(LINKER_SCRIPT)

all: $(ISO)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile C source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	gcc $(CFLAGS) -c $< -o $@

# Compile ASM source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm | $(BUILD_DIR)
	nasm -f elf $< -o $@

# Link kernel binary
$(KERNEL_BIN): $(ASM_OBJS) $(C_OBJS)
	ld $(LDFLAGS) $(ASM_OBJS) $(C_OBJS) -o $(KERNEL_BIN)

# Build ISO with GRUB config
$(ISO): $(KERNEL_BIN)
	mkdir -p $(GRUB_DIR)
	cp $(KERNEL_BIN) $(ISO_DIR)/boot/kernel.bin
	printf '%s\n' \
	'menuentry "My Kernel" {' \
	'    multiboot /boot/kernel.bin' \
	'    boot' \
	'}' > $(GRUB_DIR)/grub.cfg
	grub-mkrescue -o $(ISO) $(ISO_DIR)

run: $(ISO)
	qemu-system-i386 -debugcon stdio -cdrom $(ISO)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run clean

