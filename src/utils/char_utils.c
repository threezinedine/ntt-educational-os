#include "char_utils.h"

bool is_digit(char c)
{
	return c >= '0' && c <= '9';
}

bool is_alpha(char c)
{
	return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool is_alnum(char c)
{
	return is_alpha(c) || is_digit(c);
}