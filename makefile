CROSS_BIN_PREFIX = ~/opt/cross/bin/i386

GCC    = $(CROSS_BIN_PREFIX)-elf-gcc
GPP    = $(CROSS_BIN_PREFIX)-elf-g++
LD     = $(CROSS_BIN_PREFIX)-elf-ld
GDB    = gdb-multiarch
OBJCPY = $(CROSS_BIN_PREFIX)-elf-objcopy
QEMU   = qemu-system-i386
NASM   = nasm
DINASM = ndisasm

NASM_FLAGS    = -f elf32
NDISASM_FLAGS = -b 32
GCC_FLAGS     = -m32 -ffreestanding -O0 -Wall -Wextra -Isrc -g

BUILD_DIR 	    = build
BUILD_TEMP_DIR  = $(BUILD_DIR)/temp
BUILD_DEBUG_DIR = $(BUILD_DIR)/debug

BOOT_SRC = boot.asm
BOOT_BIN = $(BUILD_TEMP_DIR)/boot.bin

TARGET_BIN = $(BUILD_DIR)/os.bin

KERNEL_BIN_ELF = $(BUILD_TEMP_DIR)/kernel.elf
KERNEL_BIN     = $(BUILD_TEMP_DIR)/kernel.bin

ASM_SRCS = $(wildcard src/**/*.asm src/*.asm)
ASM_OBJS = $(ASM_SRCS:src/%.asm=$(BUILD_TEMP_DIR)/%.asm.o)

GCC_SRCS = $(wildcard src/**/**/*.c src/**/*.c src/*.c)
GCC_OBJS = $(GCC_SRCS:src/%.c=$(BUILD_TEMP_DIR)/%.c.o)

FILE_SYSTEM_FILE = assets/default_fs.asm
FILE_SYSTEM_BIN  = $(BUILD_TEMP_DIR)/default_fs.bin

$(TARGET_BIN): $(BOOT_SRC) $(KERNEL_BIN) $(FILE_SYSTEM_BIN)
	$(NASM) -f bin $< -o $(BOOT_BIN)
	dd if=/dev/zero bs=512 count=40 >> $(KERNEL_BIN)
	truncate -s %512 $(KERNEL_BIN)
	$(DINASM) $(NDISASM_FLAGS) $(KERNEL_BIN) > $(BUILD_DEBUG_DIR)/kernel.asm
	cat $(BOOT_BIN) $(KERNEL_BIN) $(FILE_SYSTEM_BIN) > $(TARGET_BIN)

$(FILE_SYSTEM_BIN): $(FILE_SYSTEM_FILE)
	$(NASM) -f bin $< -o $@
	dd if=/dev/zero bs=512 count=80 >> $(FILE_SYSTEM_BIN)
	truncate -s %512 $(FILE_SYSTEM_BIN)

$(KERNEL_BIN): $(ASM_OBJS) $(GCC_OBJS)
	$(LD) -T linker.ld $^ -o $(KERNEL_BIN_ELF)
	$(OBJCPY) -O binary $(KERNEL_BIN_ELF) $@

$(BUILD_TEMP_DIR)/%.asm.o: src/%.asm setup
	@mkdir -p $(dir $@)
	$(NASM) ${NASM_FLAGS} $< -o $@

$(BUILD_TEMP_DIR)/%.c.o: src/%.c setup
	@mkdir -p $(dir $@)
	$(GCC) $(GCC_FLAGS) -c $< -o $@

setup:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(BUILD_TEMP_DIR)
	mkdir -p $(BUILD_DEBUG_DIR)

qemu: $(TARGET_BIN)
	$(QEMU) -drive file=$(TARGET_BIN),format=raw,index=0,media=disk

qemu-debug: $(TARGET_BIN)
	$(QEMU) -drive file=$(TARGET_BIN),format=raw,index=0,media=disk -S -s

gdb: $(TARGET_BIN)
	$(GDB) -ex "target remote :1234" -ex "set disassembly-flavor intel" -ex "set architecture i386" -ex "break *0x7C00" -ex "continue"

gdb-c: $(TARGET_BIN)
	$(GDB) -ex "target remote :1234" -ex "set disassembly-flavor intel" -ex "set architecture i386" -ex "add-symbol-file $(KERNEL_BIN_ELF) 0x500" -ex "yes" -ex "layout src"


show: $(TARGET_BIN)
	ghex $(TARGET_BIN)

clean:
	rm -rf $(BUILD_DIR)