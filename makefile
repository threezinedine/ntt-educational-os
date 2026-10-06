GCC  = ~/opt/cross/bin/i686-elf-gcc
GPP  = ~/opt/cross/bin/i686-elf-g++
LD   = ~/opt/cross/bin/i686-elf-ld
GDB  = ~/opt/cross/bin/i686-elf-gdb
NASM = nasm

BUILD_DIR 	    = build
BUILD_TEMP_DIR  = $(BUILD_DIR)/temp
BUILD_DEBUG_DIR = $(BUILD_DIR)/debug

BOOT_SRC = boot.asm
BOOT_BIN = $(BUILD_TEMP_DIR)/boot.bin

TARGET_BIN = $(BUILD_DIR)/os.bin

$(TARGET_BIN): $(BOOT_SRC) setup
	$(NASM) -f bin $< -o $(BOOT_BIN)
	cat $(BOOT_BIN) > $(TARGET_BIN)

setup:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_TEMP_DIR)
	mkdir -p $(BUILD_DEBUG_DIR)

qemu: $(TARGET_BIN)
	qemu-system-i386 -fda $(TARGET_BIN)

qemu-debug: $(TARGET_BIN)
	qemu-system-i386 -fda $(TARGET_BIN) -S -s

gdb: $(TARGET_BIN)
	$(GDB) -ex "target remote :1234" -ex "set disassembly-flavor intel"

clean:
	rm -rf $(BUILD_DIR)