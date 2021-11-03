#include "Private.h"

unsigned int PRIVATE pobf_GenerateHash32(char* pStr)
{
   unsigned int h;
   unsigned char* p;

   h = 0;
   for (p = (unsigned char*)pStr; *p != '\0'; p++)
      h = (37 * h) + *p;

   return h;
}

uint32_t PRIVATE pobf_sys_dbg_write_process_memory(uint32_t pid, void* address, const void* data, size_t size)
{
   system_call_4(905, (uint64_t)pid, (uint64_t)address, (uint64_t)size, (uint64_t)data);
   return_to_user_prog(uint32_t);
}

uint32_t PRIVATE pobf_sys_hen_write_process_memory(uint32_t pid, void* address, const void* data, size_t size)
{
   system_call_6(8, 0x7777, 0x32, (uint64_t)pid, (uint64_t)address, (uint64_t)data, (uint64_t)size);
   return_to_user_prog(uint32_t);
}

void PRIVATE pobf_WriteProcess(void* destination, void* source, int size)
{
   static bool useHenSyscalls = false;

   if (!useHenSyscalls)
   {
      uint32_t write = pobf_sys_dbg_write_process_memory(sys_process_getpid(), destination, source, size);
      if (write == SUCCEEDED)
         return;
   }

   useHenSyscalls = true;
   pobf_sys_hen_write_process_memory(sys_process_getpid(), destination, source, size);
}

void PRIVATE pobf_Write(int pAddress, int pData, int pLength)
{
   pobf_WriteProcess((void*)pAddress, (void*)pData, pLength);
}

bool PRIVATE pobf_Skip(int pOperation)
{
   char skip_bytes[] =
   {
      0x30,
      0x3C,
      0x80,
      0xF8
   };

   for (unsigned int i = 0; i < sizeof(skip_bytes) / sizeof(skip_bytes[0]); i++)
   {
      if (((pOperation >> 24) == skip_bytes[i]))
         return true;
   }

   return false;
}