#include "Internal.h"

int gSeed;
void pobf_SetApiKey(char* pKey)
{
	gSeed = pobf_GenerateHash32(pKey);
}

void pobf_SetApiKey(int32_t key)
{
	gSeed = key;
}

void pobf_DecryptFunction(int pFunction)
{
	pFunction = *(int*)pFunction;
	Random<0x11111111, 0x66666666, 0x7FFFFFFF> rr(gSeed);

	int operation;
	int key;
	do
	{
		key = rr.next();
		operation = *(int*)pFunction;
		if (!pobf_Skip(operation))
		{
			int code = operation ^ key;
			pobf_Write(pFunction, (int)&code);
		}
		pFunction += 0x04;
	} while (operation != (0x4E800020 ^ key));
}

void pobf_DecryptData()
{
	int* ptr = (int*)decryptData_START;
	for (int i = 0; i < decryptData_SIZE; i++, ptr++)
	{
		int code = *ptr ^ (decryptData_CODE * (i + 1));
		pobf_Write((int)ptr, (int)&code);
	}
}

int32_t pobf_HashFile32()
{
	return 0;
}