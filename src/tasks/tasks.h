#ifndef _TASKS_H_
#define _TASKS_H_

#include "paging/paging.h"
#include "utils/utils.h"

enum task_state
{
	TASK_STATE_RUNNING,
	TASK_STATE_WAITING,
	TASK_STATE_TERMINATED
};

struct task
{
	uint32_t		 pid;
	enum task_state	 state;
	uint32_t		 kernel_esp;
	uint32_t		 kernel_stack_top;
	page_directory_t page_directory; // has the memory setup here
	struct task*	 pNext;
};

#endif /* _TASKS_H_ */