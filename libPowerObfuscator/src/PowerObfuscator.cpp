#include "PowerObfuscator.h"

namespace pobf
{
    char PRIV_API_KEY[] = "TEST KEY XXX";

    int EXPORTS_TOC[] =
    {
       START_PATTERN
    };

    uint32_t g_Seed = 0;

    void SetApiKey(uint32_t key)
    {
        g_Seed = key;
    }

    uint32_t MixTimeSeed(clock_t a, time_t b, sys_pid_t c)
    {
        a = a - b;  a = a - c;  a = a ^ (c >> 13);
        b = b - c;  b = b - a;  b = b ^ (a << 8);
        c = c - a;  c = c - b;  c = c ^ (b >> 13);
        a = a - b;  a = a - c;  a = a ^ (c >> 12);
        b = b - c;  b = b - a;  b = b ^ (a << 16);
        c = c - a;  c = c - b;  c = c ^ (b >> 5);
        a = a - b;  a = a - c;  a = a ^ (c >> 3);
        b = b - c;  b = b - a;  b = b ^ (a << 10);
        c = c - a;  c = c - b;  c = c ^ (b >> 15);
        return c;
    }

    uint32_t StringToHash32(const char* str)
    {
        uint32_t hash = 0;
        for (uint8_t* p = (uint8_t*)str; *p != '\0'; p++)
            hash = (37 * hash) + *p;

        return hash;
    }

    void todo_SeedRandom(uint32_t seed)
    {

    }

    int todo_Random()
    {
        return 0;
    }

    void Start()
    {
        todo_SeedRandom(MixTimeSeed(clock(), time(NULL), sys_process_getpid()));

        uint32_t random = todo_Random();
        uint32_t keyHash = StringToHash32(PRIV_API_KEY);

        SetApiKey(random + keyHash);

        DecryptAll();
    }

    void DecryptAll()
    {
        sys_prx_segment_info_t segment = GetModuleSegmentInfo(nullptr, TEXT_SEGMENT);

        for (int i = 0; i < segment.memsz; i++)
            DecryptTextSegment(segment.base);

        DecryptDataSegment();
    }

    void DecryptTextSegment(uint32_t function)
    {
        // function = ((opd_s*)function)->func;  // segment.base doesn't have opd section???
        pobf_Random<0x11111111, 0x66666666, 0x7FFFFFFF> random(g_Seed);

        uint32_t instruction = 0;
        uint32_t key = 0;
        do
        {
            key = random.Next();
            instruction = *(uint32_t*)function; // get instructions
            if (!Skip(instruction))
            {
                int code = instruction ^ key;
                pobf_write_process_memory((void*)function, &code, 4); // PPC64 instructions are 4 bytes in length 
            }
            function += 0x04;
        } while (instruction != (0x4E800020 ^ key));
    }

    void DecryptDataSegment()
    {
        sys_prx_segment_info_t segment = GetModuleSegmentInfo(nullptr, DATA_SEGMENT);
        uint32_t* ptr = (uint32_t*)segment.base;

        for (int i = 0; i < segment.memsz; i++, ptr++)
        {
            uint32_t code = *ptr ^ (segment.base * (i + 1));
            pobf_write_process_memory(ptr, &code, 4);
        }
    }

    bool Skip(uint32_t instruction)
    {
        char skip_bytes[] =
        {
           0x30,
           0x3C,
           0x80,
           0xF8
        };

        int size = sizeof(skip_bytes) / sizeof(skip_bytes[0]);
        for (int i = 0; i < size; i++)
        {
            if ((instruction >> 24) == skip_bytes[i])
                return true;
        }

        return false;
    }







    namespace Encrypt
    {
        bool DataCompare(const uint8_t* pbData, const uint8_t* pbMask, const char* szMask)
        {
            for (; *szMask; ++szMask, ++pbData, ++pbMask)
            {
                if (*szMask == 'x' && *pbData != *pbMask)
                {
                    return false;
                }
            }
            return (*szMask == NULL);
        }

        bool FindPattern(uintptr_t address, uint32_t length, uint8_t step, uint8_t* bytes, const char* mask, uint32_t* foundOffset)
        {
            for (uint32_t i = 0; i < length; i += step)
            {
                if (DataCompare((uint8_t*)(address + i), bytes, mask))
                {
                    *foundOffset = (uintptr_t)(address + i);
                    return true;
                }
            }

            *foundOffset = 0;
            return false;
        }

        void FindPatternsInParallel(uintptr_t address, uint32_t length, std::vector<Pattern>& patterns, std::vector<uint32_t>& foundOffsets)
        {
            for (auto& pat : patterns)
            {
                if (pat.found)
                    continue;

                uint32_t offset = 0;
                if (FindPattern(address, length, 8, (uint8_t*)pat.find, pat.mask, &offset))
                {
                    foundOffsets.push_back(offset);
                    pat.found = true;
                }

                if (foundOffsets.size() == 0)
                    break;
            }
        }

        void DecryptFunction(uint8_t* data, uint32_t startIndex, uint32_t endIndex, bool quick)
        {
            static uint8_t key[57];
            key[0] = 0x32;
            key[1] = 0x11;
            key[2] = 0x07;
            key[3] = 0x8A;
            key[4] = 0xE9;
            key[5] = 0x5B;
            key[6] = 0x90;
            key[7] = 0xC3;
            key[8] = 0x06;
            key[9] = 0x02;
            key[10] = 0x8E;
            key[11] = 0x78;
            key[12] = 0x09;
            key[13] = 0x8E;
            key[14] = 0xB2;
            key[15] = 0x24;
            key[16] = 0xD4;
            key[17] = 0xD1;
            key[18] = 0x14;
            key[19] = 0x06;
            key[20] = 0x81;
            key[21] = 0x34;
            key[22] = 0x76;
            key[23] = 0xE7;
            key[24] = 0x7E;
            key[25] = 0x30;
            key[26] = 0x28;
            key[27] = 0xC1;
            key[28] = 0x6D;
            key[29] = 0xFB;
            key[30] = 0x59;
            key[31] = 0x3C;
            key[32] = 0x9F;
            key[33] = 0x3D;
            key[34] = 0x9D;
            key[35] = 0xA8;
            key[36] = 0x8F;
            key[37] = 0x3A;
            key[38] = 0x85;
            key[39] = 0x2D;
            key[40] = 0x4D;
            key[41] = 0x53;
            key[42] = 0xE6;
            key[43] = 0x9B;
            key[44] = 0xED;
            key[45] = 0xFB;
            key[46] = 0xF2;
            key[47] = 0x01;
            key[48] = 0x24;
            key[49] = 0x0C;
            key[50] = 0xCF;
            key[51] = 0x53;
            key[52] = 0x7E;
            key[53] = 0x74;
            key[54] = 0x42;
            key[55] = 0xFA;
            key[56] = 0x86;

            for (uint32_t i = startIndex; i < endIndex; i++)
            {
                if (i % 4 == 0 || i % 4 == 1)
                {
                    uint8_t n[1] = { data[i] ^ key[(i - startIndex) % sizeof(key)] };
                    if (quick)
                    {
                        uint32_t address = ((uint32_t)data + i);
                        *(uint8_t*)address = n[0];
                    }
                    else
                    {
                        pobf_write_process_memory(data + i, n, 1);
                    }
                }
            }
        }


        namespace DefaultEncrypt
        {
            void __encryptFunctionStart(void* function, bool quick)
            {
                uint32_t startAddr = ((opd_s*)function)->func;
                //printf("Address: 0x%X\n",startAddr);

                std::vector<uint32_t> foundOffsets;
                foundOffsets.reserve(2); // reserve 2 to improve performance

                std::vector<Pattern> patterns = {
                    { "\x7C\x63\x22\x78\x7C\x63\x22\x78", "xxxxxxxx", false },  // xor r3, r3, r4       xor r3, r3, r4
                    { "\x7C\x63\x2A\x78\x7C\x63\x2A\x78", "xxxxxxxx", false }   // xor r3, r3, r5       xor r3, r3, r5
                };

                FindPatternsInParallel(startAddr, 0x02F00000, patterns, foundOffsets);
                //printf("foundOffsets[0] 0x%X\n", foundOffsets[0]);
                //printf("foundOffsets[1] 0x%X\n", foundOffsets[1]);


                DecryptFunction((uint8_t*)startAddr, foundOffsets[0] + 8, foundOffsets[1], quick);
                //printf("Reversed area successfully!\n");
            }


            void _encryptFunctionStart(void* function, bool quick)
            {
                __encryptFunctionStart(function, quick);
                DefaultPatternStart();
            }

            void __encryptFunctionEnd(void* function, bool quick, bool deleteData)
            {
                uint32_t startAddr = ((opd_s*)function)->func;

                std::vector<uint32_t> foundOffsets;
                foundOffsets.reserve(2); // reserve 2 to improve performance

                std::vector<Pattern> patterns = {
                    { "\x7C\x63\x22\x78\x7C\x63\x22\x78", "xxxxxxxx", false },  // xor r3, r3, r4       xor r3, r3, r4
                    { "\x7C\x63\x2A\x78\x7C\x63\x2A\x78", "xxxxxxxx", false }   // xor r3, r3, r5       xor r3, r3, r5
                };

                FindPatternsInParallel(startAddr, 0x02F00000, patterns, foundOffsets);
                //printf("foundOffsets[0] 0x%X\n", foundOffsets[0]);
                //printf("foundOffsets[1] 0x%X\n", foundOffsets[1]);

                DecryptFunction((uint8_t*)startAddr, foundOffsets[0] + 8, foundOffsets[1], quick);

                if (deleteData)
                {
                    uint8_t* d = (uint8_t*)startAddr;
                    for (uint32_t i = foundOffsets[0]; i < foundOffsets[1]; i++)
                    {
                        pobf_Random<0x11111111, 0x66666666, 0x7FFFFFFF> random(g_Seed);
                        d[i] = random.Next();
                    }
                }
            }

            void _encryptFunctionEnd(void* function, bool quick, bool deleteData)
            {
                DefaultPatternEnd();
                __encryptFunctionEnd(function, quick, deleteData);
            }
        }

        namespace QuickEncrypt
        {
            void __encryptFunctionStart(void* function, uint8_t* saveBuffer, uint32_t* start, uint32_t* end)
            {
                uint32_t startAddr = ((opd_s*)function)->func;

                std::vector<uint32_t> foundOffsets;
                foundOffsets.reserve(2); // reserve 2 to improve performance

                std::vector<Pattern> patterns = {
                    { "\x7C\x63\x32\x78\x7C\x63\x32\x78", "xxxxxxxx", false },  // xor r3, r3, r6       xor r3, r3, r6
                    { "\x7C\x63\x3A\x78\x7C\x63\x3A\x78", "xxxxxxxx", false }   // xor r3, r3, r7       xor r3, r3, r7
                };

                if (*start == 0)
                {
                    // do initial setup
                    FindPatternsInParallel(startAddr, 0x02F00000, patterns, foundOffsets);

                    *start = foundOffsets[0] + 8 + startAddr;
                    *end = foundOffsets[1] + startAddr;

                    //printf("Required min buffer size: %i\n",*end - *start);

                    // copy code to save buffer
                    memcpy(saveBuffer, (void*)*start, *end - *start);

                }

                uint32_t startAddress = *start;
                uint32_t endAddress = *end;

                DecryptFunction(saveBuffer, 0, endAddress - startAddress, true); // do fast decrypt on buffer
                pobf_write_process_memory((void*)startAddress, saveBuffer, endAddress - startAddress); // copy over the decrypted contents back over the function
                DecryptFunction(saveBuffer, 0, endAddress - startAddress, true); // do fast encrypt on buffer
            }

            void _encryptFunctionStart(void* function, uint8_t* saveBuffer, uint32_t* start, uint32_t* end)
            {
                __encryptFunctionStart(function, saveBuffer, start, end);
                QuickPatternStart();
            }

            void _encryptFunctionEnd(uint8_t* saveBuffer, uint32_t start, uint32_t end)
            {
                QuickPatternEnd();
                pobf_write_process_memory((void*)start, saveBuffer, end - start); // copy over the encrypted contents back over the function

            }
        }

        namespace InlineEncrypt
        {
            void __encryptFunctionStart(void* function)
            {
                uint32_t startAddr = ((opd_s*)function)->func;

                std::vector<uint32_t> foundOffsets;
                foundOffsets.reserve(2); // reserve 2 to improve performance

                std::vector<Pattern> patterns = {
                    { "\x7C\x63\x22\x78\x7C\x63\x22\x78", "xxxxxxxx", false },  // xor r3, r3, r4       xor r3, r3, r4
                    { "\x7C\x63\x2A\x78\x7C\x63\x2A\x78", "xxxxxxxx", false }   // xor r3, r3, r5       xor r3, r3, r5
                };

                FindPatternsInParallel(startAddr, 0x02F00000, patterns, foundOffsets);
                //printf("foundOffsets[0] 0x%X\n", foundOffsets[0]);
                //printf("foundOffsets[1] 0x%X\n", foundOffsets[1]);


                DecryptFunction((uint8_t*)startAddr, foundOffsets[0] + 8, foundOffsets[1]);
            }

            void _encryptFunctionStart(void* function)
            {
                __encryptFunctionStart(function);
                DefaultPatternStart();
            }

            void __encryptFunctionEnd(void* function)
            {
                uint32_t startAddr = ((opd_s*)function)->func;

                std::vector<uint32_t> foundOffsets;
                foundOffsets.reserve(2); // reserve 2 to improve performance

                std::vector<Pattern> patterns = {
                    { "\x7C\x63\x22\x78\x7C\x63\x22\x78", "xxxxxxxx", false },  // xor r3, r3, r4       xor r3, r3, r4
                    { "\x7C\x63\x2A\x78\x7C\x63\x2A\x78", "xxxxxxxx", false }   // xor r3, r3, r5       xor r3, r3, r5
                };

                FindPatternsInParallel(startAddr, 0x02F00000, patterns, foundOffsets);
                //printf("foundOffsets[0] 0x%X\n", foundOffsets[0]);
                //printf("foundOffsets[1] 0x%X\n", foundOffsets[1]);

                DecryptFunction((uint8_t*)startAddr, foundOffsets[0] + 8, foundOffsets[1]);
            }

            void _encryptFunctionEnd(void* function)
            {
                DefaultPatternEnd();
                __encryptFunctionEnd(function);
            }
        }




}