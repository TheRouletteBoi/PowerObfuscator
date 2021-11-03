#include <sys/syscall.h>
#include <sys/process.h>

#define PRIVATE __attribute__((visibility("hidden")))

unsigned int pobf_GenerateHash32(char *pStr);
void pobf_Write(int pAddress, int pData, int pLength = sizeof(int));
bool pobf_Skip(int pOperation);