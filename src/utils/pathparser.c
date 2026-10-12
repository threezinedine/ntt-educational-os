#include "pathparser.h"
#include "string.h"

struct path from_c_string(const char* cstr)
{
	struct path p = {cstr, cstr};
	return p;
}

static void get_buffer_from_char(char* start, char* buffer, bool include_slash);

void path_get_root(struct path* p, char* buffer)
{
	get_buffer_from_char(p->path, buffer, true);
}

void path_get_current(struct path* p, char* buffer)
{
	get_buffer_from_char(p->current, buffer, false);
}

bool path_next(struct path* p)
{
	int32_t next_slash = index_of(p->current, '/');

	if (next_slash == -1)
	{
		return false;
	}

	p->current += next_slash + 1;

	return true;
}

static void get_buffer_from_char(char* start, char* buffer, bool include_slash)
{
	int32_t first_slash = index_of(start, '/');
	if (first_slash != -1)
	{
		if (include_slash)
		{
			memcpy(buffer, start, first_slash + 1);
			buffer[first_slash + 1] = '\0';
		}
		else
		{
			memcpy(buffer, start, first_slash);
			buffer[first_slash] = '\0';
		}
	}
	else
	{
		memcpy(buffer, start, strlen(start) + 1);
	}
}