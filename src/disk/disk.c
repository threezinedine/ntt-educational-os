#include "disk.h"
#include "utils/utils.h"

#define DATA			 0
#define ERROR_R			 1
#define SECTOR_COUNT	 2
#define LBA_LOW			 3
#define LBA_MID			 4
#define LBA_HIGH		 5
#define DRIVE_SELECT	 6
#define COMMAND_REGISTER 7
#define STATUS_REGISTER	 7

// Control register defines
#define CONTROL 0x206

#define ALTERNATE_STATUS 0

#define ATA_PORT 0x1F0

#define READ_SECTORS_COMMAND  0x20
#define WRITE_SECTORS_COMMAND 0x30
#define FLUSH_CACHE_COMMAND	  0xE7

static void ata_io_wait(const uint16_t port);

error_t disk_read(bool slave, uint32_t lba, uint32_t sectors, uint8_t* buffer)
{
	error_t result = OK;

	if (buffer == NULL)
	{
		result = ERROR_NULL_POINTER;
		goto out;
	}

	outb(ATA_PORT + SECTOR_COUNT, sectors & 0xFF);
	outb(ATA_PORT + LBA_LOW, lba & 0xFF);
	outb(ATA_PORT + LBA_MID, (lba >> 8) & 0xFF);
	outb(ATA_PORT + LBA_HIGH, (lba >> 16) & 0xFF);
	outb(ATA_PORT + DRIVE_SELECT, 0xE0 | (slave << 4) | ((lba >> 24) & 0x0F));
	outb(ATA_PORT + COMMAND_REGISTER, READ_SECTORS_COMMAND); // Read sectors command

	ata_io_wait(ATA_PORT);

	while ((inb(ATA_PORT + STATUS_REGISTER) & 0x80) != 0); // Wait until the drive is not busy
	while ((inb(ATA_PORT + STATUS_REGISTER) & 0x08) == 0); // Wait until the drive is ready to transfer data

	for (uint32_t i = 0; i < sectors; i++)
	{
		insw(ATA_PORT + DATA, (uint16_t*)(buffer + i * 512), 256);
	}

out:
	return result;
}

void ata_write_sector(bool slave, uint32_t lba, const void* buffer)
{
	// 1. Wait if controller is busy
	while (inb(ATA_PORT + STATUS_REGISTER) & 0x80);

	// 2. Select Master Drive + top 4 bits of LBA
	outb(ATA_PORT + DRIVE_SELECT, 0xE0 | (slave << 4) | ((lba >> 24) & 0x0F));

	// 3. Write 1 sector count and LBA address bytes
	outb(ATA_PORT + SECTOR_COUNT, 1);
	outb(ATA_PORT + LBA_LOW, (uint8_t)(lba));
	outb(ATA_PORT + LBA_MID, (uint8_t)(lba >> 8));
	outb(ATA_PORT + LBA_HIGH, (uint8_t)(lba >> 16));

	// 4. Send 0x30 (WRITE SECTORS)
	outb(ATA_PORT + COMMAND_REGISTER, WRITE_SECTORS_COMMAND);

	// 5. Wait until Drive is NOT busy (0x80) AND ready for Data (0x08)
	while ((inb(ATA_PORT + STATUS_REGISTER) & 0x88) != 0x08);

	// 6. Push 256 words (512 bytes)
	outsw(ATA_PORT + DATA, buffer, 256);

	// 7. Flush cache
	outb(ATA_PORT + COMMAND_REGISTER, FLUSH_CACHE_COMMAND);
	while (inb(ATA_PORT + STATUS_REGISTER) & 0x80);
}

error_t disk_write(bool slave, uint32_t lba, uint32_t sectors, uint8_t* buffer)
{
	error_t result = OK;

	if (buffer == NULL)
	{
		result = ERROR_NULL_POINTER;
		goto out;
	}

	for (uint32_t i = 0; i < sectors; i++)
	{
		ata_write_sector(slave, lba + i, buffer + i * 512);
	}

out:
	return result;
}

error_t disk_flush(void)
{
	error_t result = OK;

out:
	return result;
}

static void ata_io_wait(const uint16_t port)
{
	inb(port + CONTROL + ALTERNATE_STATUS);
	inb(port + CONTROL + ALTERNATE_STATUS);
	inb(port + CONTROL + ALTERNATE_STATUS);
	inb(port + CONTROL + ALTERNATE_STATUS);
}