#ifndef __POWEROBFUSCATOR_H__
#define __POWEROBFUSCATOR_H__

#include <types.h>
#include <time.h>
#include <string.h>
#undef vector
#include <vector>
#include "Defines.h"

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

    namespace EncryptV2
    {
        bool DataCompare(const uint8_t* pbData, const uint8_t* pbMask, const char* szMask);
        bool FindPattern(uintptr_t address, uint32_t length, uint8_t step, uint8_t* bytes, const char* mask, uint32_t* foundOffset);
        void FindPatternsInParallel(uintptr_t address, uint32_t length, std::vector<Pattern>& patterns, std::vector<uint32_t>& foundOffsets);
        void DecryptFunction(uint8_t* data, uint32_t startIndex, uint32_t endIndex, bool quick = false);

        namespace Default
        {
            void __encryptFunctionStart(void* function, bool quick);
            void _encryptFunctionStart(void* function, bool quick);
            void __encryptFunctionEnd(void* function, bool quick, bool deleteData = false);
            void _encryptFunctionEnd(void* function, bool quick, bool deleteData = false);
        }

        namespace Quick
        {
            void __encryptFunctionStart(void* function, uint8_t* saveBuffer, uint32_t* start, uint32_t* end);
            void _encryptFunctionStart(void* function, uint8_t* saveBuffer, uint32_t* start, uint32_t* end);
            void _encryptFunctionEnd(uint8_t* saveBuffer, uint32_t start, uint32_t end);
        }

        namespace Inline
        {
            void __encryptFunctionStart(void* function);
            void _encryptFunctionStart(void* function);
            void __encryptFunctionEnd(void* function);
            void _encryptFunctionEnd(void* function);
        }

        /******* For pattern scanning ********/
#define StartatternDefault() \
            __asm("xor %r3, %r3, %r4;"       /* \x7C\x63\x22\x78 */    \
                    "xor %r3, %r3, %r4;");   /* \x7C\x63\x22\x78 */

#define EndPatternDefault() \
            __asm("xor %r3, %r3, %r5;"       /* \x7C\x63\x2A\x78 */   \
                    "xor %r3, %r3, %r5;");   /* \x7C\x63\x2A\x78 */

#define StartPatternQuick() \
            __asm("xor %r3, %r3, %r6;"       /* \x7C\x63\x32\x78 */   \
                    "xor %r3, %r3, %r6;");   /* \x7C\x63\x32\x78 */

#define EndPatternQuick() \
            __asm("xor %r3, %r3, %r7;"       /* \x7C\x63\x3A\x78 */   \
                    "xor %r3, %r3, %r7;");   /* \x7C\x63\x3A\x78 */

#define StartPattternThread() \
            __asm("lis %r3, 0xFFEE;"          /* \x3C\x60\xFF\xEE */ \
                    "ori %r3, %r3, 0xDDCC;"); /* \x60\x63\xDD\xCC */

#define EndPatternThread() \
            __asm("lis %r3, 0xFFEE;"          /* \x3C\x60\xFF\xEE */ \
                    "ori %r3, %r3, 0xDDCD;"); /* \x60\x63\xDD\xCD */



    }


}

#endif // __POWEROBFUSCATOR_H__

