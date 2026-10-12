#ifndef _PATHPARSER_H_
#define _PATHPARSER_H_

#include "common.h"

struct path
{
	char* path;
	char* current;
};

struct path from_c_string(const char* cstr);

void path_get_root(struct path* p, char* buffer);
void path_get_current(struct path* p, char* buffer);
bool path_next(struct path* p);

#endif /* _PATHPARSER_H_ */