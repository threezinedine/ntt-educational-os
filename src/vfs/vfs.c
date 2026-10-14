#include "vfs.h"

struct virtual_file_system
{
	struct file_system* pSystems[10];
	uint32_t			system_count;
};

static struct virtual_file_system g_vfs;

error_t vfs_init()
{
	error_t result = OK;

	memset(&g_vfs, 0, sizeof(g_vfs));

out:
	return result;
}

error_t vfs_register_file_system(struct file_system* pSystem)
{
	error_t result = OK;

	if (pSystem == NULL)
	{
		result = ERROR_NULL_POINTER;
		goto out;
	}

	if (g_vfs.system_count >= 10)
	{
		result = ERROR_EXCEED_NUMBER_OF_FILE_SYSTEMS;
		goto out;
	}

	if (pSystem->resolve == NULL || pSystem->open == NULL || pSystem->close == NULL)
	{
		result = ERROR_INVALID_ARGUMENT;
		goto out;
	}

	g_vfs.pSystems[g_vfs.system_count++] = pSystem;

out:
	return result;
}

error_t vfs_unregister_file_system(struct file_system* pSystem)
{
	error_t result = OK;

	if (g_vfs.system_count == 0)
	{
		result = ERROR_EXCEED_NUMBER_OF_FILE_SYSTEMS;
		goto out;
	}

	if (pSystem == NULL)
	{
		result = ERROR_NULL_POINTER;
		goto out;
	}

	// find the system and remove it
	for (uint32_t i = 0; i < g_vfs.system_count; i++)
	{
		if (g_vfs.pSystems[i] == pSystem)
		{
			for (uint32_t j = i; j < g_vfs.system_count - 1; j++)
			{
				g_vfs.pSystems[j] = g_vfs.pSystems[j + 1];
			}
			g_vfs.pSystems[g_vfs.system_count - 1] = NULL;
			g_vfs.system_count--;
			break;
		}
	}

out:
	return result;
}

error_t vfs_shutdown()
{
	error_t result = OK;

	memset(&g_vfs, 0, sizeof(g_vfs));

out:
	return result;
}

struct file_description vfs_open(struct path path)
{
	struct file_description file = {};
	file.is_open				 = false;

	for (uint32_t i = 0; i < g_vfs.system_count; i++)
	{
		if (g_vfs.pSystems[i]->resolve(g_vfs.pSystems[i], path))
		{
			file.pSystem = g_vfs.pSystems[i];
			break;
		}
	}

	if (file.pSystem == NULL)
	{
		file.is_open = false;
		goto out;
	}

	if (file.pSystem == NULL)
	{
		file.is_open = false;
		goto out;
	}

	error_t open_result = file.pSystem->open(file.pSystem, path, &file);
	if (open_result != OK)
	{
		file.is_open = false;
		file.pSystem = NULL;
		goto out;
	}
	file.is_open = true;

out:
	return file;
}

error_t vfs_close(struct file_description* pFile)
{
	error_t result = OK;

	if (pFile == NULL)
	{
		result = ERROR_NULL_POINTER;
		goto out;
	}

	if (pFile->pSystem != NULL)
	{
		pFile->pSystem->close(pFile->pSystem, pFile);
	}

	pFile->pSystem = NULL;
	pFile->is_open = false;

out:
	return result;
}