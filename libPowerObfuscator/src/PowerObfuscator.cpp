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


}