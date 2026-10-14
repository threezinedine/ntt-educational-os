#ifndef _VFS_H_
#define _VFS_H_

#include "utils/utils.h"

struct file_system;
struct file_description;
typedef bool (*vfs_resolve_fn)(struct file_system*, struct path);
typedef error_t (*vfs_open_fn)(struct file_system*, struct path, struct file_description*);
typedef error_t (*vfs_close_fn)(struct file_system*, struct file_description*);

struct file_description
{
	bool				is_open;
	struct file_system* pSystem;
	void*				pFD;
};

struct file_system
{
	vfs_resolve_fn resolve;
	vfs_open_fn	   open;
	vfs_close_fn   close;
	void*		   pInternal;
};

error_t vfs_init();
error_t vfs_register_file_system(struct file_system* pSystem);
error_t vfs_unregister_file_system(struct file_system* pSystem);
error_t vfs_shutdown();

struct file_description vfs_open(struct path path);
error_t					vfs_close(struct file_description* pFile);

#endif // _VFS_H_
