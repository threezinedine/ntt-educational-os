#include "fs.h"
#include "memory/memory.h"
#include "utils/utils.h"

struct fs_internal
{
};

static bool	   fs_resolve(struct file_system* pSystem, struct path path);
static error_t fs_open(struct file_system* pSystem, struct path path, struct file_description* pFile);
static error_t fs_close(struct file_system* pSystem, struct file_description* pFile);

struct file_system* fs_create()
{
	struct file_system* pSystem	  = (struct file_system*)kzalloc(sizeof(struct file_system));
	struct fs_internal* pInternal = (struct fs_internal*)kzalloc(sizeof(struct fs_internal));

	pSystem->pInternal = pInternal;
	pSystem->resolve   = fs_resolve;
	pSystem->open	   = fs_open;
	pSystem->close	   = fs_close;

	return pSystem;
}

error_t fs_destroy(struct file_system* pSystem)
{
	struct fs_internal* pInternal = (struct fs_internal*)pSystem->pInternal;
	kfree(pInternal);
	kfree(pSystem);

	return OK;
}

static bool fs_resolve(struct file_system* pSystem, struct path path)
{
	return true;
}

static error_t fs_open(struct file_system* pSystem, struct path path, struct file_description* pFile)
{
	error_t result = OK;
	return result;
}

static error_t fs_close(struct file_system* pSystem, struct file_description* pFile)
{
	error_t result = OK;
	return result;
}