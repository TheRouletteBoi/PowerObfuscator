#ifndef __POWEROBFUSCATOR_H__
#define __POWEROBFUSCATOR_H__

#include <types.h>
#include <time.h>
#include <string.h>
#undef vector
#include <vector>
#include "Defines.h"
#include "Memory/Memory.h"
#include "Module/Module.h"

namespace pobf
{
    struct opd_s
    {
        uint32_t func;
        uint32_t toc;
    };

    struct Pattern
    {
        const char* find;
        const char* mask;
        bool found;
    };

    #define START_PATTERN 0xAABBCCDD, 0x12345678, 0xEEFFEEFF 

    extern char PRIV_API_KEY[];
    extern int EXPORTS_TOC[];

    void        POBF_API    SetApiKey(uint32_t key);
    uint32_t    POBF_API    MixTimeSeed(clock_t a, time_t b, sys_pid_t c);
    void        POBF_API    todo_SeedRandom(uint32_t seed);
    int         POBF_API    todo_Random();
    uint32_t    POBF_API    StringToHash32(const char* str);
    void        POBF_API    Start();
    void        POBF_API    DecryptAll();
    void        POBF_API    DecryptTextSegment(uint32_t function);
    void        POBF_API    DecryptDataSegment();
    bool        POBF_API    Skip(uint32_t instruction);

}

#endif // __POWEROBFUSCATOR_H__

