#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <sys/syscall.h>
#include <sys/process.h>
#include "Defines.h"


void POBF_API pobf_write_process_memory(void* destination, const void* source, size_t size);




#endif // __MEMORY_H__