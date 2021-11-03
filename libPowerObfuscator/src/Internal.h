#pragma once
#include <types.h>
#include "libPowerObfuscator.h"

void pobf_SetApiKey(char *pKey);
void pobf_SetApiKey(int32_t key);
void pobf_DecryptFunction(int pFunction);
void pobf_DecryptData();
int32_t pobf_HashFile32();