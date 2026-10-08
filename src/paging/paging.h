#ifndef _PAGING_H_
#define _PAGING_H_

#include "utils/utils.h"

typedef void* page_directory_t;
#define INVALID_PAGE_DIRECTORY ((void*)0)

error_t paging_init();

page_directory_t paging_create_page_directory(uint32_t size, uint8_t root_page);
error_t			 paging_enable_paging();
error_t			 paging_switch_page_directory(page_directory_t page_directory);
error_t			 paging_shutdown();

#endif // _PAGING_H_