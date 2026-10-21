#include "tasks.h"
#include "disk/disk.h"
#include "gdt/gdt.h"
#include "memory/memory.h"
#include "utils/utils.h"

static uint32_t s_next_pid = 1;

#define KERNEL_STACK_SIZE 4096
#define SIZE_64_MB		  0x4000000
#define SIZE_128_MB		  0x8000000

#define USER_LAND_START SIZE_64_MB

#if 0
#define DEFAULT_USER_EFLAGS 0x0202
#else
#define DEFAULT_USER_EFLAGS 0x0002
#endif

struct task* task_create(const char* exe)
{
	struct task* pTask		= (struct task*)kzalloc(sizeof(struct task));
	pTask->pid				= s_next_pid++;
	pTask->state			= TASK_STATE_WAITING;
	pTask->kernel_stack_top = (uint32_t)kzalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
	pTask->kernel_esp		= pTask->kernel_stack_top;
	pTask->page_directory	= page_create_page_directory_with_share(0x0, SIZE_64_MB, SIZE_64_MB);
	pTask->pNext			= NULL;

	// Find the executable in the memory
	pTask->pNext			 = NULL;
	pTask->code_sector_index = 0;
	pTask->code_sector_count = 0;
	pTask->code_start		 = USER_LAND_START + 4 + strlen(exe) + 3;
	pTask->loaded			 = 0;

	uint8_t buffer[512];
	while (pTask->code_sector_index < 100)
	{
		memclear(buffer, 512);
		disk_read(0, pTask->code_sector_index, 1, buffer);

		if (strncmp((const char*)buffer, "bin.", 4) != 0)
		{
			pTask->code_sector_index++;
			continue;
		}

		if (strncmp((const char*)(buffer + 4), exe, strlen(exe)) != 0)
		{
			pTask->code_sector_index++;
			continue;
		}

		pTask->code_sector_count = buffer[4 + strlen(exe) + 1] - '0';
		break;
	}

	return pTask;
}

static void load_task_code(struct task* pTask);

error_t task_run(struct task* pTask)
{
	error_t result = OK;

	if (pTask == NULL)
	{
		result = ERROR_INVALID_ARGUMENT;
		goto out;
	}

	page_switch_page_directory(pTask->page_directory);

	if (pTask->loaded == 0)
	{
		load_task_code(pTask);
		pTask->loaded = 1;
	}

	set_tss_esp0(pTask->kernel_esp);

	// switch to user land
	switch_to_user_land(
		pTask->code_start, USER_CODE_SEGMENT, DEFAULT_USER_EFLAGS, USER_LAND_START + SIZE_64_MB, USER_DATA_SEGMENT);

out:
	return result;
}

static void load_task_code(struct task* pTask)
{
	uint32_t user_start = USER_LAND_START;
	disk_read(0, pTask->code_sector_index, pTask->code_sector_count, (uint8_t*)user_start);
}

void task_destroy(struct task* pTask)
{
	kfree((void*)(pTask->kernel_stack_top - KERNEL_STACK_SIZE));
	page_destroy_page_directory(pTask->page_directory);

	kfree(pTask);
}