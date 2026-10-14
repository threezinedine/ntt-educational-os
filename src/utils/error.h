#ifndef _ERROR_H_
#define _ERROR_H_

#include <stdint.h>
typedef uint8_t error_t;

#define OK									0
#define ERROR_INVALID_INTERRUPT_NUMBER		1
#define ERROR_INVALID_ARGUMENT				2
#define ERROR_MISALIGNED_BLOCK				3
#define ERROR_OUT_OF_MEMORY					4
#define ERROR_NULL_POINTER					5
#define ERROR_EXCEED_NUMBER_OF_FILE_SYSTEMS 6
#define ERROR_FILE_NOT_FOUND				7
#define ERROR_MISSING_REQUIRED_FIELD		8

#endif /* _ERROR_H_ */