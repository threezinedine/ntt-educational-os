#ifndef _PMM_H_
#define _PMM_H_

#include "utils/utils.h"

error_t pmm_init(uint32_t segments_count);
void*	pmm_get_free_segment();
void	pmm_free_segment(void* segment);
error_t pmm_shutdown();

#endif // _PMM_H_