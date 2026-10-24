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
	uint32_t		 code_sector_index;
	uint32_t		 code_sector_count;
	uint32_t		 code_start;
	bool			 loaded;
};

struct task* task_create(const char* exe);
error_t		 task_run(struct task* pTask);
void		 task_destroy(struct task* pTask);
extern void
prepare_user_land(uint32_t* pKernel_esp, uint32_t eip, uint32_t cs, uint32_t efflags, uint32_t esp, uint32_t ss);
extern void switch_to_user_land(uint32_t kernel_esp);
extern void internal_switch_to_user_land(uint32_t kernel_esp, uint32_t* p_prev_kernel_esp);
void		task_switch();

#endif /* _TASKS_H_ */