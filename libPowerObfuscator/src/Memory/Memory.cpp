#include "Memory.h"

uint32_t pobf_sys_dbg_write_process_memory(sys_pid_t pid, void* destination, const void* source, size_t size)
{
    system_call_4(905, (uint64_t)pid, (uint64_t)destination, (uint64_t)size, (uint64_t)source);
    return_to_user_prog(uint32_t);
}

uint32_t pobf_ps3mapi_write_process_memory(sys_pid_t pid, void* destination, const void* source, size_t size)
{
    system_call_6(8, 0x7777, 0x32, (uint64_t)pid, (uint64_t)destination, (uint64_t)source, (uint64_t)size);
    return_to_user_prog(uint32_t);
}

void pobf_write_process_memory(void* destination, const void* source, size_t size)
{
    static bool usePs3mapiSyscalls = false;

    if (!usePs3mapiSyscalls)
    {
        uint32_t write = pobf_sys_dbg_write_process_memory(sys_process_getpid(), destination, source, size);
        if (write == SUCCEEDED)
            return;
    }

    usePs3mapiSyscalls = true;
    pobf_ps3mapi_write_process_memory(sys_process_getpid(), destination, source, size);
}