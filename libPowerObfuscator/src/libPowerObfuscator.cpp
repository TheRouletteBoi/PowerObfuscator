#include "libPowerObfuscator.h"

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

void pobf_DecryptAll()
{
	for (int i = 1; i < fnData.count; i++)
	{
		pobf_DecryptFunction(fnData.start + fnData.offset[i]);
	}
	pobf_DecryptData();
}

void pobf_Start(int suppressParameter)
{
	int32_t fileHash = pobf_HashFile32();
	int32_t keyHash = pobf_GenerateHash32(PRIV_API_KEY);

	pobf_SetApiKey(fileHash + keyHash);

	pobf_DecryptAll();
}