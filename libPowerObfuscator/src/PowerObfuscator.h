#ifndef __POWEROBFUSCATOR_H__
#define __POWEROBFUSCATOR_H__

#include <types.h>
#include "Defines.h"
#include "Memory/Memory.h"

namespace pobf
{
    struct __fnData
    {
        int16_t count;
        int32_t start;
        int16_t offset[256];
    };

    extern __fnData fnData;

    extern char PRIV_API_KEY[];

    extern int decryptData_START;
    extern int decryptData_SIZE;
    extern int decryptData_CODE;

    #define START_PATTERN 0xAABBCCDD, 0x12345678, 0xEEFFEEFF 
    extern int EXPORTS_TOC[];

    /*
    struct
    {
       int32_t fnData_ptr;
       int32_t decryptData_start;
       int32_t decryptData_code;
    };
    */

    void        POBF_API    SetApiKey(uint32_t key);
    uint32_t    POBF_API    GenerateHash32(const char* str);
    uint32_t    POBF_API    HashFile32();
    void        POBF_API    Start();
    void        POBF_API    DecryptAll();
    void        POBF_API    DecryptTextSegment(uint32_t function);
    void        POBF_API    DecryptDataSegment();
    bool        POBF_API    Skip(uint32_t instruction);

}

#endif // __POWEROBFUSCATOR_H__

