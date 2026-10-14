#ifndef _DISK_H_
#define _DISK_H_

#include "utils/common.h"

error_t disk_read(bool slave, uint32_t lba, uint32_t sectors, uint8_t* buffer);
error_t disk_write(bool slave, uint32_t lba, uint32_t sectors, uint8_t* buffer);
error_t disk_flush(void);

#endif // _DISK_H_