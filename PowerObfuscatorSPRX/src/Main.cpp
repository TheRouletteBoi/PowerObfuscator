#include <cellstatus.h>
#include <sys/prx.h>
#include <sys/ppu_thread.h>
#include <sys/timer.h>
#include "PowerObfuscatorAPI.h"

extern "C" int _sys_printf(const char* fmt, ...);
#define printf _sys_printf
extern "C" int _sys_snprintf(char* buffer, size_t len, const char* fmt, ...);
#define snprintf _sys_snprintf

SYS_MODULE_INFO( PowerObfuscatorSPRX, 0, 1, 1);
SYS_MODULE_START( PowerObfuscatorSPRXMain );
SYS_MODULE_STOP( PowerObfuscatorSPRXStop );


sys_ppu_thread_t gTestEncryptedThreadId = SYS_PPU_THREAD_ID_INVALID;


void ThisFuncShouldBeEncrypted001();
int ThisFuncShouldBeEncrypted002(int left, int right);
void ThisFuncShouldBeEncrypted003();

void ThisFuncShouldBeEncrypted001()
{
   int value = ThisFuncShouldBeEncrypted002(800, 255);
   printf("value: %d\n", value);

   int value2 = ThisFuncShouldBeEncrypted002(450, 2896);
   printf("value2: %d\n", value2);
}

int ThisFuncShouldBeEncrypted002(int left, int right)
{
   int value = left * right;
   return value;
}

void ThisFuncShouldBeEncrypted003()
{
   ThisFuncShouldBeEncrypted001();
   ThisFuncShouldBeEncrypted001();
}

void MainThread(uint64_t arg)
{
   ThisFuncShouldBeEncrypted001();
   int value = ThisFuncShouldBeEncrypted002(999, 999);
   printf("value in thread %D\n", value);
}

extern "C" int PowerObfuscatorSPRXMain(int argc, char* argv[])
{
   int var = EXPORTS_TOC[0];
   pobf_Start(var);

   sys_ppu_thread_create(&gTestEncryptedThreadId, MainThread, 0, 3000, 8192, SYS_PPU_THREAD_CREATE_JOINABLE, "TestEncryptedThread");

   ThisFuncShouldBeEncrypted003();
   return 0;
}

extern "C" int PowerObfuscatorSPRXStop(int argc, char* argv[])
{

   return 0;
}