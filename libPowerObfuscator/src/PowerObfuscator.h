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









    namespace Encrypt
    {
        uint32_t moduleTextSegmentStartCache = 0;
        uint32_t moduleTextSegmentEndCache = 0;


        ALWAYS_INLINE bool DataCompare(const uint8_t* pbData, const uint8_t* pbMask, const char* szMask);
        ALWAYS_INLINE bool FindPattern(uintptr_t address, uint32_t length, uint8_t step, uint8_t* bytes, const char* mask, uint32_t* foundOffset);
        ALWAYS_INLINE void FindPatternsInParallel(uintptr_t address, uint32_t length, std::vector<Pattern>& patterns, std::vector<uint32_t>& foundOffsets);
        ALWAYS_INLINE void DecryptFunction(uint8_t* data, uint32_t startIndex, uint32_t endIndex, bool quick = false);

        namespace DefaultEncrypt
        {
            void __encryptFunctionStart(void* function, bool quick);
            ALWAYS_INLINE void _encryptFunctionStart(void* function, bool quick);
            void __encryptFunctionEnd(void* function, bool quick, bool deleteData = false);
            ALWAYS_INLINE void _encryptFunctionEnd(void* function, bool quick, bool deleteData = false);
        }

        namespace QuickEncrypt
        {
            void __encryptFunctionStart(void* function, uint8_t* saveBuffer, uint32_t* start, uint32_t* end);
            ALWAYS_INLINE void _encryptFunctionStart(void* function, uint8_t* saveBuffer, uint32_t* start, uint32_t* end);
            ALWAYS_INLINE void _encryptFunctionEnd(uint8_t* saveBuffer, uint32_t start, uint32_t end);
        }

        namespace InlineEncrypt
        {
            ALWAYS_INLINE void __encryptFunctionStart(void* function);
            ALWAYS_INLINE void _encryptFunctionStart(void* function);
            ALWAYS_INLINE void __encryptFunctionEnd(void* function);
            ALWAYS_INLINE void _encryptFunctionEnd(void* function);
        }



        /******* default Encrypt ********/
#define encryptFunctionStart(function) {} \
            void *local_encrypt_function_address = function; \
            DefaultEncrypt::_encryptFunctionStart(local_encrypt_function_address, false);

#define encryptFunctionEnd() {} \
            DefaultEncrypt::_encryptFunctionEnd(local_encrypt_function_address, false);

#define encryptFunctionEnd_deletedata() {} \
            DefaultEncrypt::_encryptFunctionEnd(local_encrypt_function_address, false, true);



        /******* quickEncrypt ********/
#define encryptFunctionStart_quick(function,bufferSize) {} \
            void *local_encrypt_function_address = function; \
            static uint8_t encryptionBuffer[bufferSize]; \
            static uint32_t startEncryptAddress = 0; \
            static uint32_t endEncryptAddress = 0; \
            QuickEncrypt::_encryptFunctionStart(function, encryptionBuffer, &startEncryptAddress, &endEncryptAddress);

#define encryptFunctionEnd_quick() {} \
            QuickEncrypt::_encryptFunctionEnd(encryptionBuffer, startEncryptAddress, endEncryptAddress);


        /******* inlineEncrypt ********/
#define inline_encryptFunctionStart(function) {} \
            void *local_encrypt_function_address = function; \
            InlineEncrypt::_encryptFunctionStart(local_encrypt_function_address);

#define inline_encryptFunctionEnd() {} \
            InlineEncrypt::_encryptFunctionEnd(local_encrypt_function_address);




        /******* Patterns to scan for ********/
#define DefaultPatternStart() \
            __asm("xor %r3, %r3, %r4"       /* \x7C\x63\x22\x78 */    \
                    "xor %r3, %r3, %r4");   /* \x7C\x63\x22\x78 */

#define DefaultPatternEnd() \
            __asm("xor %r3, %r3, %r5"       /* \x7C\x63\x2A\x78 */   \
                    "xor %r3, %r3, %r5");   /* \x7C\x63\x2A\x78 */

#define QuickPatternStart() \
            __asm("xor %r3, %r3, %r6"       /* \x7C\x63\x32\x78 */   \
                    "xor %r3, %r3, %r6");   /* \x7C\x63\x32\x78 */

#define QuickPatternEnd() \
            __asm("xor %r3, %r3, %r7"       /* \x7C\x63\x3A\x78 */   \
                    "xor %r3, %r3, %r7");   /* \x7C\x63\x3A\x78 */

#define ThreadPattternStart() \
            __asm("lis %r3, 0xFFEE"          /* \x3C\x60\xFF\xEE */ \
                    "ori %r3, %r3, 0xDDCC"); /* \x60\x63\xDD\xCC */

#define ThreadPatternEnd() \
            __asm("lis %r3, 0xFFEE"          /* \x3C\x60\xFF\xEE */ \
                    "ori %r3, %r3, 0xDDCD"); /* \x60\x63\xDD\xCD */



    }


}

#endif // __POWEROBFUSCATOR_H__

