#include "PowerObfuscator.h"

namespace pobf
{
    __fnData fnData;

    char PRIV_API_KEY[] = "TEST KEY XXX";

    int decryptData_START = 0x00000000;
    int decryptData_SIZE = 0x00000000;
    int decryptData_CODE = 0x00000000;

    int EXPORTS_TOC[] =
    {
       START_PATTERN,
       (int)&fnData,
       (int)&decryptData_START,
       (int)&decryptData_SIZE,
       (int)&decryptData_CODE,
    };

    uint32_t g_Seed = 0;

    void SetApiKey(uint32_t key)
    {
        g_Seed = key;
    }

    uint32_t GenerateHash32(const char* str)
    {
        uint32_t hash = 0;
        for (uint8_t* p = (uint8_t*)str; *p != '\0'; p++)
            hash = (37 * hash) + *p;

        return hash;
    }

    uint32_t HashFile32()
    {
        //return vxRAND();
        return 0;
    }

    void Start()
    {
        uint32_t fileHash = HashFile32();
        uint32_t keyHash = GenerateHash32(PRIV_API_KEY);

        SetApiKey(fileHash + keyHash);

        DecryptAll();
    }

    void DecryptAll()
    {
        for (int i = 1; i < fnData.count; i++)
            DecryptTextSegment(fnData.start + fnData.offset[i]);

        DecryptDataSegment();
    }

    void DecryptTextSegment(uint32_t function)
    {
        // use sys_prx_module_info to get text segment

        function = *(uint32_t*)function; // Get opd_s
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
        // use sys_prx_module_info to get data segment
        uint32_t* ptr = (uint32_t*)decryptData_START;
        for (int i = 0; i < decryptData_SIZE; i++, ptr++)
        {
            uint32_t code = *ptr ^ (decryptData_CODE * (i + 1));
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