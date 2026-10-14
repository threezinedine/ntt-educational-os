#ifndef _FS_H_
#define _FS_H_

#include "utils/utils.h"
#include "vfs/vfs.h"

struct file_system* fs_create();
error_t				fs_destroy(struct file_system* pSystem);

#endif // _FS_H_