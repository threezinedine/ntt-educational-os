#ifndef _FS_H_
#define _FS_H_

#include "utils/utils.h"
#include "vfs/vfs.h"

struct file_system* fs_create();
error_t				fs_destroy(struct file_system* pSystem);

typedef uint32_t fs_item;

void	 fs_get_folder_name(struct file_system* pSystem, uint32_t folder, char* name, uint32_t name_size);
void	 fs_mkdir(struct file_system* pSystem, struct path path);
void	 fs_get_file_name(struct file_system* pSystem, uint32_t file, char* name, uint32_t name_size);
uint32_t fs_get_items_count(struct file_system* pSystem, uint32_t folder);
fs_item	 fs_get_item(struct file_system* pSystem, uint32_t folder, uint32_t index);
bool	 fs_is_folder(struct file_system* pSystem, fs_item item);

extern fs_item ROOT_FOLDER;

#endif // _FS_H_