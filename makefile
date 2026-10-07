GCC    = ~/opt/cross/bin/i686-elf-gcc
GPP    = ~/opt/cross/bin/i686-elf-g++
LD     = ~/opt/cross/bin/i686-elf-ld
GDB    = ~/opt/cross/bin/i686-elf-gdb
OBJCPY = ~/opt/cross/bin/i686-elf-objcopy
NASM   = nasm

BUILD_DIR 	    = build
BUILD_TEMP_DIR  = $(BUILD_DIR)/temp
BUILD_DEBUG_DIR = $(BUILD_DIR)/debug

BOOT_SRC = boot.asm
BOOT_BIN = $(BUILD_TEMP_DIR)/boot.bin

TARGET_BIN = $(BUILD_DIR)/os.bin

KERNEL_BIN_ELF = $(BUILD_TEMP_DIR)/kernel.elf
KERNEL_BIN     = $(BUILD_TEMP_DIR)/kernel.bin

ASM_SRCS = $(wildcard src/**/*.asm src/*.asm)
ASM_OBJS = $(ASM_SRCS:src/%.asm=$(BUILD_TEMP_DIR)/%.o)

$(TARGET_BIN): $(BOOT_SRC) $(KERNEL_BIN)
	$(NASM) -f bin $< -o $(BOOT_BIN)
	dd if=/dev/zero bs=512 count=5 >> $(KERNEL_BIN)
	truncate -s %512 $(KERNEL_BIN)
	cat $(BOOT_BIN) $(KERNEL_BIN) > $(TARGET_BIN)

$(KERNEL_BIN): $(ASM_OBJS)
	$(LD) -Ttext 0x0500 -m elf_i386 $^ -o $(KERNEL_BIN_ELF)
	$(OBJCPY) -O binary $(KERNEL_BIN_ELF) $@

$(BUILD_TEMP_DIR)/%.o: src/%.asm setup
	$(NASM) -f elf32 $< -o $@

setup:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_TEMP_DIR)
	mkdir -p $(BUILD_DEBUG_DIR)

qemu: $(TARGET_BIN)
	qemu-system-i386 -fda $(TARGET_BIN)

qemu-debug: $(TARGET_BIN)
	qemu-system-i386 -fda $(TARGET_BIN) -S -s

gdb: $(TARGET_BIN)
	$(GDB) -ex "target remote :1234" -ex "set disassembly-flavor intel" -ex "set architecture i386" -ex "break *0x7C00" -ex "continue"

clean:
	rm -rf $(BUILD_DIR)