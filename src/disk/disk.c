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

// Control register defines
#define CONTROL 0x206

#define ALTERNATE_STATUS 0

#define ATA_PORT 0x1F0

static void ata_io_wait(const uint16_t port);

error_t disk_read(bool slave, uint32_t lba, uint32_t sectors, uint32_t* buffer)
{
	error_t result = OK;

	if (buffer == NULL)
	{
		result = ERROR_NULL_POINTER;
		goto out;
	}

	volatile uint8_t read_cmd[12] = {0xA8,
									 0,
									 (lba >> 0x18) & 0xFF,
									 (lba >> 0x10) & 0xFF,
									 (lba >> 0x08) & 0xFF,
									 (lba >> 0x00) & 0xFF,
									 (sectors >> 0x18) & 0xFF,
									 (sectors >> 0x10) & 0xFF,
									 (sectors >> 0x08) & 0xFF,
									 (sectors >> 0x00) & 0xFF,
									 0,
									 0};

	outb(ATA_PORT + DRIVE_SELECT, 0xA0 | (slave << 4)); // Drive select
	ata_io_wait(ATA_PORT);
	outb(ATA_PORT + ERROR_R, 0x00);
	outb(ATA_PORT + LBA_MID, 2048 & 0xFF);
	outb(ATA_PORT + LBA_HIGH, 2048 >> 8);
	outb(ATA_PORT + COMMAND_REGISTER, 0xA0); // Packet command
	ata_io_wait(ATA_PORT);					 // I think we might need this delay, not sure, so keep this

	// Wait for status
	while (1)
	{
		uint8_t status = inb(ATA_PORT + COMMAND_REGISTER);
		if ((status & 0x01) == 1) return 1;
		if (!(status & 0x80) && (status & 0x08)) break;
		ata_io_wait(ATA_PORT);
	}

	// Send command
	outsw(ATA_PORT + DATA, (uint32_t*)read_cmd, 3);

	for (uint32_t i = 0; i < sectors; i++)
	{
		// Wait until ready
		while (1)
		{
			uint8_t status = inb(ATA_PORT + COMMAND_REGISTER);
			if (status & 0x01) return 1;
			if (!(status & 0x80) && (status & 0x08)) break;
		}

		int size = inb(ATA_PORT + LBA_HIGH) << 8 | inb(ATA_PORT + LBA_MID); // Get the size of transfer

		insw(ATA_PORT + DATA, (uint32_t*)((uint8_t*)buffer + i * 0x800), size / 2); // Read it
	}

out:
	return result;
}

error_t disk_write(bool slave, uint32_t lba, uint32_t sectors, uint32_t* buffer)
{
	error_t result = OK;

	if (buffer == NULL)
	{
		result = ERROR_NULL_POINTER;
		goto out;
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